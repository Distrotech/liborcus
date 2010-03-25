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
#include <gsf/gsf-libxml.h>
#include <gsf/gsf-opendoc-utils.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>

using namespace std;

#include <stdio.h>
#include <string>
#include <sys/time.h>

namespace {

class StackPrinter
{
public:
    explicit StackPrinter(const char* msg) :
        msMsg(msg)
    {
        fprintf(stdout, "%s: --begin\n", msMsg.c_str());
        mfStartTime = getTime();
    }

    ~StackPrinter()
    {
        double fEndTime = getTime();
        fprintf(stdout, "%s: --end (duration: %g sec)\n", msMsg.c_str(), (fEndTime-mfStartTime));
    }

    void printTime(int line) const
    {
        double fEndTime = getTime();
        fprintf(stdout, "%s: --(%d) (duration: %g sec)\n", msMsg.c_str(), line, (fEndTime-mfStartTime));
    }

private:
    double getTime() const
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }

    ::std::string msMsg;
    double mfStartTime;
};

}

void test_start (GsfXMLIn *xin, xmlChar const **attrs)
{
    StackPrinter __stack_printer__("::test_start");
    size_t i = 0;
    if (attrs)
    {
        while (attrs[i] && attrs[i+1])
        {    
            cout << attrs[i] << "=\"" << attrs[i+1] << "\"" << endl;
            i += 2;
        }
    }
}

void test_end (GsfXMLIn *xin, G_GNUC_UNUSED GsfXMLBlob *blob)
{
    StackPrinter __stack_printer__("::test_end");
}

static GsfXMLInNode const opendoc_content_dtd [] =
{
    GSF_XML_IN_NODE_FULL (START, START, -1, NULL, GSF_XML_NO_CONTENT, FALSE, TRUE, NULL, NULL, 0),
    GSF_XML_IN_NODE (START, OFFICE, OO_NS_OFFICE, "document-content", GSF_XML_NO_CONTENT, &test_start, &test_end),
    GSF_XML_IN_NODE (OFFICE, OFFICE_BODY, OO_NS_OFFICE, "body", GSF_XML_NO_CONTENT, &test_start, &test_end),
    GSF_XML_IN_NODE (OFFICE_BODY, SPREADSHEET, OO_NS_OFFICE, "spreadsheet", GSF_XML_NO_CONTENT, &test_start, &test_end),
    GSF_XML_IN_NODE_END
};

void read_content_xml(GsfInput* input, size_t size)
{
#if 1
    GsfXMLInDoc* doc = gsf_xml_in_doc_new(opendoc_content_dtd, get_gsf_ooo_ns());
    gsf_xml_in_doc_parse(doc, input, NULL);
    gsf_xml_in_doc_free (doc);
#else
    const guint8* content = gsf_input_read(input, size, NULL);
    for (size_t i = 0; i < size; ++i)
    {
        guint8 c = content[i];
        cout << c;
    }
    cout << endl;
#endif
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

    list_content (GSF_INPUT(infile));
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
