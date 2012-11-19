/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include "orcus/orcus_xml.hpp"
#include "orcus/xml_namespace.hpp"
#include "spreadsheet/factory.hpp"
#include "spreadsheet/document.hpp"

#include "xml_map_sax_handler.hpp"

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace orcus;
using namespace std;

namespace {

void print_help()
{
    cout << "Usage: orcus-xml --mode=MODE [map file] [data file] [output file (optional)]" << endl;
    cout << endl;
    cout << "The MODE must be one of dump, transform, or dump-check." << endl;
}

enum output_mode {
    unknown, dump_document, transform_xml, dump_document_check
};

output_mode parse_mode(const char* s)
{
    const char* prefix = "--mode=";
    size_t prefix_size = strlen(prefix);
    const char* p_value = NULL;
    size_t value_size = 0;
    for (size_t i = 0, n = strlen(s); i < n; ++i, ++s)
    {
        if (i < prefix_size)
        {
            if (*s != prefix[i])
                // Required prefix is not present.
                return unknown;
            continue;
        }

        if (!p_value)
            p_value = s;

        ++value_size;
    }

    if (value_size == 4 && strncmp(p_value, "dump", 4) == 0)
        return dump_document;

    if (value_size == 9 && strncmp(p_value, "transform", 9) == 0)
        return transform_xml;

    if (value_size == 10 && strncmp(p_value, "dump-check", 10) == 0)
        return dump_document_check;

    return unknown;
}

}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        print_help();
        return EXIT_FAILURE;
    }

    // Parse the mode.
    output_mode mode = parse_mode(argv[1]);
    if (mode == unknown)
    {
        cerr << "unknown mode" << endl;
        print_help();
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
    boost::scoped_ptr<spreadsheet::import_factory> import_fact(new spreadsheet::import_factory(doc.get()));
    boost::scoped_ptr<spreadsheet::export_factory> export_fact(new spreadsheet::export_factory(doc.get()));

    xmlns_repository repo;
    xmlns_context cxt = repo.create_context();
    orcus_xml app(cxt, import_fact.get(), export_fact.get());
    read_map_file(app, argv[2]);
    app.read_file(argv[3]);

    switch (mode)
    {
        case dump_document:
            doc->dump();
        break;
        case transform_xml:
        {
            if (argc <= 4)
            {
                cout << "output xml file name not provided" << endl;
                print_help();
                return EXIT_FAILURE;
            }

            // Write transformed xml content to file.
            app.write_file(argv[4]);
        }
        break;
        case dump_document_check:
        {
            if (argc <= 4)
            {
                doc->dump_check(cout);
                break;
            }

            ofstream file(argv[4]);
            if (!file)
            {
                cerr << "failed to create output file: " << argv[4] << endl;
                return EXIT_FAILURE;
            }

            doc->dump_check(file);
        }
        break;
        default:
            ;
    }

    pstring::intern::dispose();

    return EXIT_SUCCESS;
}
