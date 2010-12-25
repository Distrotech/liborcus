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

#include "xmlparser.hpp"
#include "ooxml/xlsx_handler.hpp"
#include "ooxml/opc_handler.hpp"
#include "ooxml/ooxml_tokens.hpp"
#include "global.hpp"

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
        mp_input(gsf_infile_child_by_name(GSF_INFILE(parent), name))
    {
        if (!mp_input)
        {
            ostringstream os;
            os << "failed to open infile child: '" << name << "'";
            throw general_error(os.str());
        }
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

/**
 * Parse the [Content_Types].xml part to extract the paths and content types
 * of the other xml parts contained in the package. This is the first part 
 * in the package to be parsed. 
 */
void read_content_types(
    GsfInput* input, vector<xml_part_t>& parts, vector<xml_part_t>& ext_defaults)
{
    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "name: [Content_Types].xml  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, "[Content_Types].xml");
    ::boost::scoped_ptr<opc_content_types_handler> handler(new opc_content_types_handler(opc_tokens));
    parser.set_handler(handler.get());
    parser.parse();
    handler->pop_parts(parts);
    handler->pop_ext_defaluts(ext_defaults);
}

void read_relations(GsfInput* input, const char* path)
{
    size_t size = gsf_input_size(input);
    cout << "---" << endl;
    cout << "name: " << path << "  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(opc_tokens, content, size, path);
    ::boost::scoped_ptr<opc_relations_handler> handler(new opc_relations_handler(opc_tokens));
    parser.set_handler(handler.get());
    parser.parse();
}

/**
 * Parse a sheet xml part that contains data stored in a single sheet.
 */
void read_sheet_xml(GsfInput* input, const char* outpath)
{
    size_t size = gsf_input_size(input);
    cout << "name: sheet1  size: " << size << endl;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(ooxml_tokens, content, size, "sheet.xml");
    ::boost::scoped_ptr<xlsx_sheet_xml_handler> handler(new xlsx_sheet_xml_handler(ooxml_tokens));
    parser.set_handler(handler.get());
    parser.parse();
}

/**
 * The top-level function that determines the order in which the individual 
 * parts get parsed. 
 */
void read_content(GsfInput* input, const char* outpath)
{
    if (!GSF_IS_INFILE(input))
        return;

    // [Content_Types].xml

    vector<xml_part_t> parts;
    vector<xml_part_t> ext_defaults;
    {
        gsf_infile_guard xml_content_types_guard(input, "[Content_Types].xml");
        GsfInput* xml_content_types = xml_content_types_guard.get();
        read_content_types(xml_content_types, parts, ext_defaults);
    }
    for_each(parts.begin(), parts.end(), print_xml_content_types("part name"));
    for_each(ext_defaults.begin(), ext_defaults.end(), print_xml_content_types("extension default"));

    // _rels/.rels

    {
        gsf_infile_guard dir_rels_guard(input, "_rels");
        GsfInput* dir_rels = dir_rels_guard.get();
        gsf_infile_guard rels_guard(dir_rels, ".rels");
        GsfInput* rels = rels_guard.get();
        read_relations(rels, "_rels/.rels");
    }

    // xl/worksheets/sheet1.xml

    gsf_infile_guard dir_xl_guard(input, "xl");
    GsfInput* dir_xl = dir_xl_guard.get();

    gsf_infile_guard dir_worksheets_guard(dir_xl, "worksheets");
    GsfInput* dir_worksheets = dir_worksheets_guard.get();

    cout << "---" << endl;
    gsf_infile_guard xml_sheet1_guard(dir_worksheets, "sheet1.xml");
    GsfInput* xml_sheet1 = xml_sheet1_guard.get();
    read_sheet_xml(xml_sheet1, outpath);
}

void list_content (GsfInput* input, int level = 0)
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

void read_file(const char* fpath, const char* outpath)
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

//  list_content(GSF_INPUT(infile));
    read_content (GSF_INPUT(infile), outpath);
    g_object_unref (G_OBJECT (infile));
}

}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    gsf_init();
    read_file(argv[1], "out.html");
    gsf_shutdown();
    pstring::intern::dispose();
    return EXIT_SUCCESS;
}
