/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include <gsf/gsf-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-zip.h>

#include "global.hpp"
#include "xml_parser.hpp"
#include "xml_simple_handler.hpp"
#include "ooxml/global.hpp"
#include "ooxml/xlsx_handler.hpp"
#include "ooxml/xlsx_context.hpp"
#include "ooxml/opc_context.hpp"
#include "ooxml/ooxml_tokens.hpp"
#include "ooxml/schemas.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace orcus;

namespace {

class print_xml_content_types : unary_function<void, xml_part_t>
{
public:
    print_xml_content_types(const char* prefix) :
        m_prefix(prefix) {}

    void operator() (const xml_part_t& v) const
    {
        cout << "* " << m_prefix << ": " << v.first;
        if (v.second)
            cout << " (" << v.second << ")";
        else
            cout << " (<unknown content type>)";
        cout << endl;
    }
private:
    const char* m_prefix;
};

class gsf_infile_guard
{
public:
    gsf_infile_guard(GsfInput* parent, const char* name) : 
        mp_input(NULL)
    {
        if (name)
            mp_input = gsf_infile_child_by_name(GSF_INFILE(parent), name);
        else
            mp_input = parent;

        if (!mp_input)
        {
            ostringstream os;
            os << "failed to open infile child: '" << name << "'";
            throw general_error(os.str());
        }
    }

    gsf_infile_guard(const gsf_infile_guard& r) :
        mp_input(r.mp_input)
    {
        if (mp_input)
            g_object_ref(G_OBJECT(mp_input));
    }

    ~gsf_infile_guard() 
    {
        if (mp_input)
            g_object_unref(G_OBJECT(mp_input)); 
    }

    GsfInput* get() const { return mp_input; }

private:
    GsfInput* mp_input;
};

class orcus_xlsx
{
public:
    orcus_xlsx();
    ~orcus_xlsx();

    void read_file(const char* fpath, const char* outpath);

    /**
     * Read an xml part inside package.  The path is relative to the relation 
     * file. 
     * 
     * @param path the path to the xml part.
     * @param type schema type.
     */
    void read_part(const pstring& path, const schema_t type);

private:
    /**
     * Parse the [Content_Types].xml part to extract the paths and content 
     * types of the other xml parts contained in the package. This is the 
     * first part in the package to be parsed. 
     */
    void read_content_types();

    void read_relations(const char* path, vector<opc_rel_t>& rels);

    void read_workbook(const char* file_name);

    /**
     * Parse a sheet xml part that contains data stored in a single sheet.
     */
    void read_sheet_xml(GsfInput* input, const char* outpath);

    /**
     * The top-level function that determines the order in which the 
     * individual parts get parsed. 
     */
    void read_content(const char* outpath);

    void list_content (GsfInput* input, int level = 0);

private:
    xml_simple_stream_handler m_opc_rel_handler;

    vector<xml_part_t> m_parts;
    vector<xml_part_t> m_ext_defaults;

    vector<gsf_infile_guard> m_dir_stack;
};

orcus_xlsx::orcus_xlsx() :
    m_opc_rel_handler(new opc_relations_context(opc_tokens)) {}

orcus_xlsx::~orcus_xlsx() {}

struct process_opc_rel : public unary_function<void, opc_rel_t>
{
    process_opc_rel(orcus_xlsx& parent) : m_parent(parent) {}

    void operator() (const opc_rel_t& v)
    {
        m_parent.read_part(v.target, v.type);
    }
private:
    orcus_xlsx& m_parent;
};

void orcus_xlsx::read_file(const char* fpath, const char* outpath)
{
    cout << "reading " << fpath << endl;

    GError* err = NULL;
    GsfInput* input = gsf_input_stdio_new (fpath, &err);
    if (!input)
    {    
        g_error_free (err);
        return;
    }

    GsfInfile* infile = gsf_infile_zip_new (input, &err);
    g_object_unref (G_OBJECT (input));
    if (!infile)
    {
        g_error_free (err);
        return;
    }

    gsf_infile_guard dir_root(GSF_INPUT(infile), NULL);
    m_dir_stack.push_back(dir_root);
//  list_content(GSF_INPUT(infile));
    read_content (outpath);
}

void orcus_xlsx::read_part(const pstring& path, schema_t type)
{
    assert(!m_dir_stack.empty());
    
    // Used for unwinding at the end of this method.
    size_t stack_size = m_dir_stack.size();

    // Change current directory and read the in-file.
    const char* p = path.get();
    const char* p_name = NULL;
    size_t name_len = 0;
    for (size_t i = 0, n = path.size(); i < n; ++i, ++p, ++name_len)
    {
        if (!p_name)
            p_name = p;

        if (*p == '/')
        {
            // Push a new directory.
            string dir_name(p_name, name_len);
            gsf_infile_guard dir_new(m_dir_stack.back().get(), dir_name.c_str());
            m_dir_stack.push_back(dir_new);
            
            p_name = NULL;
            name_len = 0;
        }
    }

    if (p_name)
    {
        // This is a file.
        string file_name(p_name, name_len);

        if (type == SCH_rels_office_doc)
            read_workbook(file_name.c_str());
        else
        {
            cout << "---" << endl;
            cout << "unhandled schema type: " << type << endl;
        }
    }

    // Unwind to the original directory.
    vector<gsf_infile_guard>::iterator itr = m_dir_stack.begin();
    advance(itr, stack_size);
    m_dir_stack.erase(itr, m_dir_stack.end());
}

void orcus_xlsx::read_content_types()
{
    gsf_infile_guard guard(m_dir_stack.back().get(), "[Content_Types].xml");
    GsfInput* input = guard.get();
    if (!input)
        throw general_error("could not open [Content_Types].xml part.");

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "name: [Content_Types].xml  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, "[Content_Types].xml");
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new opc_content_types_context(opc_tokens)));
    opc_content_types_context& context = 
        static_cast<opc_content_types_context&>(handler->get_context());
    parser.set_handler(handler.get());
    parser.parse();
    context.pop_parts(m_parts);
    context.pop_ext_defaults(m_ext_defaults);
}

void orcus_xlsx::read_relations(const char* path, vector<opc_rel_t>& rels)
{
    GsfInput* dir_cur = m_dir_stack.back().get();
    gsf_infile_guard rels_guard(dir_cur, path);
    GsfInput* input_rels = rels_guard.get();

    size_t size = gsf_input_size(input_rels);
    cout << "---" << endl;
    cout << "name: " << path << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input_rels, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, path);

    opc_relations_context& context = 
        static_cast<opc_relations_context&>(m_opc_rel_handler.get_context());
    context.init();
    parser.set_handler(&m_opc_rel_handler);
    parser.parse();
    context.pop_rels(rels);
}

void orcus_xlsx::read_workbook(const char* file_name)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();
    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_workbook_context(ooxml_tokens)));
    xlsx_workbook_context& context = 
        static_cast<xlsx_workbook_context&>(handler->get_context());
    parser.set_handler(handler.get());
    parser.parse();
}

void orcus_xlsx::read_sheet_xml(GsfInput* input, const char* outpath)
{
    size_t size = gsf_input_size(input);
    cout << "name: sheet1  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, "sheet.xml");
    ::boost::scoped_ptr<xlsx_sheet_xml_handler> handler(new xlsx_sheet_xml_handler(ooxml_tokens));
    parser.set_handler(handler.get());
    parser.parse();
}

void orcus_xlsx::read_content(const char* outpath)
{
    if (m_dir_stack.empty())
        return;

    // Get the root directory handle.
    GsfInput* dir_root = m_dir_stack.back().get();

    // [Content_Types].xml

    read_content_types();
    for_each(m_parts.begin(), m_parts.end(), print_xml_content_types("part name"));
    for_each(m_ext_defaults.begin(), m_ext_defaults.end(), print_xml_content_types("extension default"));

    // _rels/.rels

    vector<opc_rel_t> rels;

    gsf_infile_guard dir_rels(dir_root, "_rels");
    m_dir_stack.push_back(dir_rels);
    read_relations(".rels", rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this));

#if 0
    // xl/worksheets/sheet1.xml

    gsf_infile_guard dir_xl_guard(dir_root, "xl");
    GsfInput* dir_xl = dir_xl_guard.get();

    gsf_infile_guard dir_worksheets_guard(dir_xl, "worksheets");
    GsfInput* dir_worksheets = dir_worksheets_guard.get();

    cout << "---" << endl;
    gsf_infile_guard xml_sheet1_guard(dir_worksheets, "sheet1.xml");
    GsfInput* xml_sheet1 = xml_sheet1_guard.get();
    read_sheet_xml(xml_sheet1, outpath);
#endif
}

void orcus_xlsx::list_content (GsfInput* input, int level)
{
    if (!GSF_IS_INFILE(input))
        return;

    int child_count = gsf_infile_num_children (GSF_INFILE (input));
    bool is_dir = child_count >= 0;
    
    for (int i = 0; i < level; ++i)
        printf("   ");

    const char* name = gsf_input_name(input);
    size_t size = gsf_input_size(input);
    if (name)
    {
        cout << "name = " << name << " (size: " << size << ")" << endl;
    }

    if (!is_dir)
        return;

    for (int i = 0; i < level; ++i)
        printf("   ");

    puts ("{");
    for (int i = 0 ; i < child_count; ++i)
    {    
        GsfInput* child = gsf_infile_child_by_index (GSF_INFILE (input), i);
        list_content(child, level+1);
        g_object_unref(G_OBJECT(child));
    }

    for (int i = 0; i < level; ++i)
        printf("   ");
    puts ("}");
}

}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    gsf_init();
    orcus_xlsx app;
    app.read_file(argv[1], "out.html");
    gsf_shutdown();
//  pstring::intern::dump();
    pstring::intern::dispose();
    return EXIT_SUCCESS;
}
