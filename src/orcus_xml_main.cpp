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
#include "spreadsheet/factory.hpp"
#include "spreadsheet/document.hpp"

#include "xml_map_sax_handler.hpp"

#include <boost/scoped_ptr.hpp>

#include <cstdlib>

using namespace orcus;
using namespace std;

namespace {

void print_help()
{
    cout << "Usage: orcus-xml [map file] [data file] [transformed data file (optional)]" << endl;
}

}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        print_help();
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
    boost::scoped_ptr<spreadsheet::import_factory> import_fact(new spreadsheet::import_factory(doc.get()));
    boost::scoped_ptr<spreadsheet::export_factory> export_fact(new spreadsheet::export_factory(doc.get()));

    orcus_xml app(import_fact.get(), export_fact.get());
    read_map_file(app, argv[1]);
    app.read_file(argv[2]);
    if (argc > 3)
        // Write transformed xml content to file.
        app.write_file(argv[3]);
    else
        // Dump document content to stdout.
        doc->dump();

    pstring::intern::dispose();

    return EXIT_SUCCESS;
}
