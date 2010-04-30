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
//#include "odf/odshandler.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace orcus;

void read_sheet_xml(GsfInput* input, size_t size, const char* outpath)
{
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(content, size, "sheet.xml");
//  ::boost::scoped_ptr<ods_content_xml_handler> handler(new ods_content_xml_handler);
//  parser.set_handler(handler.get());
//  parser.parse();
//  handler->print_html(outpath);
}

void read_content(GsfInput* input, const char* outpath)
{
    if (!GSF_IS_INFILE(input))
        return;

    GsfInput* dir_xl = gsf_infile_child_by_name (GSF_INFILE (input), "xl");
    if (!dir_xl)
    {
        cout << "failed to get xl directory" << endl;
        return;
    }

    GsfInput* dir_worksheets = gsf_infile_child_by_name(GSF_INFILE(dir_xl), "worksheets");
    g_object_unref(G_OBJECT(dir_xl));
    if (!dir_worksheets)
    {
        cout << "failed to get worksheets directory" << endl;
        return;
    }

    GsfInput* xml_sheet1 = gsf_infile_child_by_name(GSF_INFILE(dir_worksheets), "sheet1.xml");
    g_object_unref(G_OBJECT(dir_worksheets));
    if (!xml_sheet1)
    {
        cout << "failed to get sheet1 stream" << endl;
        return;
    }

    size_t size = gsf_input_size(xml_sheet1);
    cout << "name: sheet1  size: " << size << endl;
    read_sheet_xml(xml_sheet1, size, outpath);
    g_object_unref(G_OBJECT(xml_sheet1));
}

#if 1
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
#endif

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
    if (!infile)
    {
        g_error_free (err);
        return;
    }

//  list_content(GSF_INPUT(infile));
    read_content (GSF_INPUT(infile), outpath);
    g_object_unref (G_OBJECT (infile));
    g_object_unref (G_OBJECT (input));
}

int main(int argc, char** argv)
{
    if (argc != 3)
        return EXIT_FAILURE;

    gsf_init();
    read_file(argv[1], argv[2]);
    gsf_shutdown();

    return EXIT_SUCCESS;
}
