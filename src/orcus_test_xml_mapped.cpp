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
#include "orcus/global.hpp"
#include "model/factory.hpp"
#include "model/document.hpp"

#include "xml_map_sax_handler.hpp"

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace orcus;

const char* files[] = {
    "../test/xml-mapped/basic"
};

void test_mapped_xml_import()
{
    string strm;
    size_t n = sizeof(files)/sizeof(files[0]);
    for (size_t i = 0; i < n; ++i)
    {
        string base_name(files[i]);
        string data_file = base_name + ".xml";
        string map_file = base_name + "-map.xml";
        string check_file = base_name + ".check";

        // Load the data file content.
        cout << "reading " << data_file << endl;
        load_file_content(data_file.c_str(), strm);

        boost::scoped_ptr<model::document> doc(new model::document);
        boost::scoped_ptr<model::factory> fact(new model::factory(doc.get()));

        // Parse the map file to define map rules, and parse the data file.
        orcus_xml app(fact.get());
        read_map_file(app, map_file.c_str());
        app.read_file(data_file.c_str());

        // Check the content of the document against static check file.
        ostringstream os;
        doc->dump_check(os);
        string loaded = os.str();
        load_file_content(check_file.c_str(), strm);

        assert(!loaded.empty());
        assert(!strm.empty());

        pstring p1(&loaded[0], loaded.size()), p2(&strm[0], strm.size());

        p1 = p1.trim();
        p2 = p2.trim();
        assert(p1 == p2);
    }
}

int main()
{
    test_mapped_xml_import();
    return EXIT_SUCCESS;
}
