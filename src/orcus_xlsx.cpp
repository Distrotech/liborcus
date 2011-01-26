/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

#include "orcus/global.hpp"
#include "orcus/xml_parser.hpp"
#include "orcus/xml_simple_handler.hpp"
#include "orcus/ooxml/global.hpp"
#include "orcus/ooxml/xlsx_handler.hpp"
#include "orcus/ooxml/xlsx_context.hpp"
#include "orcus/ooxml/opc_context.hpp"
#include "orcus/ooxml/ooxml_tokens.hpp"
#include "orcus/ooxml/schemas.hpp"
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/sheet.hpp"
#include "orcus/model/factory.hpp"
#include "orcus/model/document.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

#define USE_FILE_BUFFER 0
#if USE_FILE_BUFFER
#include <fcntl.h>
#include <sys/mman.h>
#endif

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;
using namespace orcus;

namespace {

#if USE_FILE_BUFFER
struct temp_buf
{
    guint8* buf;
    size_t  size;
    char    name[18];
};

bool create_buffer(temp_buf& obj, size_t size)
{
    strcpy(obj.name, "/tmp/orcus-XXXXXX");
    int fd = mkostemp(obj.name, O_RDWR | O_CREAT | O_TRUNC);

    if (fd == -1)
        return false;

    do
    {
        if (lseek(fd, size - 1, SEEK_SET) == -1)
            break;

        if (write(fd, "", 1) != 1)
            break;

        obj.buf = (guint8*)mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (obj.buf == MAP_FAILED)
            break;

        obj.size = size;
        return true;
    }
    while (false);

    close(fd);
    return false;
}

void destroy_buffer(temp_buf& obj)
{
    munmap(obj.buf, obj.size);
    unlink(obj.name);
}
#endif

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

struct print_sheet_info : unary_function<void, pair<pstring, const opc_rel_extra*> >
{
    void operator() (const pair<pstring, const opc_rel_extra*>& v) const
    {
        const xlsx_rel_sheet_info* info = static_cast<const xlsx_rel_sheet_info*>(v.second);
        cout << "sheet name: " << info->name << "  sheet id: " << info->id << "  relationship id: " << v.first << endl;
    }
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

class orcus_xlsx : public ::boost::noncopyable
{
public:
    orcus_xlsx(model::factory_base* factory);
    ~orcus_xlsx();

    void read_file(const char* fpath, const char* outpath);

    /**
     * Read an xml part inside package.  The path is relative to the relation 
     * file. 
     * 
     * @param path the path to the xml part.
     * @param type schema type.
     */
    void read_part(const pstring& path, const schema_t type, const opc_rel_extra* data);

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
    void read_sheet(const char* file_name, const xlsx_rel_sheet_info* data);

    /**
     * Parse sharedStrings.xml part that contains a list of strings referenced
     * in the document.
     */
    void read_shared_strings(const char* file_name);

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

    ::boost::shared_ptr<model::factory_base> mp_factory;
};

orcus_xlsx::orcus_xlsx(model::factory_base* factory) :
    m_opc_rel_handler(new opc_relations_context(opc_tokens)),
    mp_factory(factory) {}

orcus_xlsx::~orcus_xlsx() {}

struct process_opc_rel : public unary_function<void, opc_rel_t>
{
    process_opc_rel(orcus_xlsx& parent, const opc_rel_extras_t* extras) : 
        m_parent(parent), m_extras(extras) {}

    void operator() (const opc_rel_t& v)
    {
        const opc_rel_extra* data = NULL;
        if (m_extras)
        {
            opc_rel_extras_t::const_iterator itr = m_extras->find(v.rid);
            if (itr != m_extras->end())
                data = itr->second;
        }
        m_parent.read_part(v.target, v.type, data);
    }
private:
    orcus_xlsx& m_parent;
    const opc_rel_extras_t* m_extras;
};

void orcus_xlsx::read_file(const char* fpath, const char* outpath)
{
    cout << "reading " << fpath << endl;

    gsf_init();

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

    gsf_shutdown();
}

void orcus_xlsx::read_part(const pstring& path, schema_t type, const opc_rel_extra* data)
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

        if (type == SCH_od_rels_office_doc)
            read_workbook(file_name.c_str());
        else if (type == SCH_od_rels_worksheet)
            read_sheet(file_name.c_str(), static_cast<const xlsx_rel_sheet_info*>(data));
        else if (type == SCH_od_rels_shared_strings)
            read_shared_strings(file_name.c_str());
        else
        {
            cout << "---" << endl;
            cout << "unhandled relationship type: " << type << endl;
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

    // Get sheet info from the context instance.
    opc_rel_extras_t sheet_data;
    context.pop_sheet_info(sheet_data);
    for_each(sheet_data.begin(), sheet_data.end(), print_sheet_info());

    // Read the relationship file associated with this file, located at
    // _rels/<file name>.rels.
    vector<opc_rel_t> rels;
    gsf_infile_guard dir_rels(m_dir_stack.back().get(), "_rels");
    m_dir_stack.push_back(dir_rels);
    string rels_file_name(file_name);
    rels_file_name += ".rels";
    read_relations(rels_file_name.c_str(), rels);
    m_dir_stack.pop_back();

    for_each(rels.begin(), rels.end(), print_opc_rel());
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, &sheet_data));
}

void orcus_xlsx::read_sheet(const char* file_name, const xlsx_rel_sheet_info* data)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    if (data)
    {
        cout << "relationship sheet data: " << endl;
        cout << "  sheet name: " << data->name << "  sheet ID: " << data->id << endl;
    }
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    model::sheet_base* sheet = mp_factory->append_sheet(data->name.get(), data->name.size());
    ::boost::scoped_ptr<xlsx_sheet_xml_handler> handler(new xlsx_sheet_xml_handler(ooxml_tokens, sheet));
    parser.set_handler(handler.get());
    parser.parse();
}

void orcus_xlsx::read_shared_strings(const char* file_name)
{
    gsf_infile_guard guard(m_dir_stack.back().get(), file_name);
    GsfInput* input = guard.get();

    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "file name: " << file_name << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);

    xml_stream_parser parser(ooxml_tokens, content, size, file_name);
    ::boost::scoped_ptr<xml_simple_stream_handler> handler(
        new xml_simple_stream_handler(new xlsx_shared_strings_context(ooxml_tokens, mp_factory->get_shared_strings())));
    xlsx_shared_strings_context& context = 
        static_cast<xlsx_shared_strings_context&>(handler->get_context());
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
    for_each(rels.begin(), rels.end(), process_opc_rel(*this, NULL));
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

    ::boost::scoped_ptr<model::document> doc(new model::document);

    orcus_xlsx app(new model::factory(doc.get()));
    app.read_file(argv[1], "out.html");
    doc->dump();
//  pstring::intern::dump();
    pstring::intern::dispose();
    return EXIT_SUCCESS;
}
