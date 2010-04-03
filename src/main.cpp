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
#include "odshandler.hpp"
#include "odscontext.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace orcus;

void read_content_xml(GsfInput* input, size_t size)
{
    ods_content_xml_context context;
    const guint8* content = gsf_input_read(input, size, NULL);
    xml_stream_parser parser(content, size, "content.xml");
    ::boost::scoped_ptr<ods_content_xml_handler> handler(new ods_content_xml_handler(&context));
    parser.set_handler(handler.get());
    parser.parse();
}

void read_content(GsfInput* input)
{
    if (!GSF_IS_INFILE(input))
        return;

    GsfInput* content_xml = gsf_infile_child_by_name (GSF_INFILE (input), "content.xml");
    if (content_xml)
    {
        const char* name = gsf_input_name(content_xml);
        size_t size = gsf_input_size(content_xml);
        cout << "name: " << name << "  size: " << size << endl;
        read_content_xml(content_xml, size);
        g_object_unref(G_OBJECT(content_xml));
    }
}

#if 0
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
        if (!strncmp(name, "content.xml", 11))
            read_content_xml(input, size);
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

void read_file(const char* fpath)
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

    read_content (GSF_INPUT(infile));
    g_object_unref (G_OBJECT (infile));
    g_object_unref (G_OBJECT (input));
}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    gsf_init();
    read_file(argv[1]);
    gsf_shutdown();

    return EXIT_SUCCESS;
}
