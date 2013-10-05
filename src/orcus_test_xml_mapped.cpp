/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_xml.hpp"
#include "orcus/global.hpp"
#include "orcus/sax_ns_parser.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/stream.hpp"

#include "orcus/spreadsheet/factory.hpp"
#include "orcus/spreadsheet/document.hpp"

#include "xml_map_sax_handler.hpp"
#include "dom_tree_sax_handler.hpp"

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/scoped_ptr.hpp>

#include <unistd.h>

using namespace std;
using namespace orcus;

namespace {

struct test_case
{
    const char* base_dir;
    bool output_equals_input;
};

const test_case tests[] =
{
    { SRCDIR"/test/xml-mapped/attribute-basic", true },
    { SRCDIR"/test/xml-mapped/attribute-namespace", true },
    { SRCDIR"/test/xml-mapped/attribute-range-self-close", true },
    { SRCDIR"/test/xml-mapped/attribute-single-element", true },
    { SRCDIR"/test/xml-mapped/attribute-single-element-2", true },
    { SRCDIR"/test/xml-mapped/content-basic", true },
    { SRCDIR"/test/xml-mapped/content-namespace", false },
    { SRCDIR"/test/xml-mapped/content-namespace-2", true },
};

const char* temp_output_xml = "out.xml";

void dump_xml_structure(string& dump_content, string& strm, const char* filepath, xmlns_context& cxt)
{
    load_file_content(filepath, strm);
    dom_tree_sax_handler hdl(cxt);
    sax_ns_parser<dom_tree_sax_handler> parser(strm.c_str(), strm.size(), cxt, hdl);
    parser.parse();
    ostringstream os;
    hdl.dump_compact(os);
    dump_content = os.str();
}

void test_mapped_xml_import()
{
    string strm;
    size_t n = sizeof(tests)/sizeof(tests[0]);
    for (size_t i = 0; i < n; ++i)
    {
        string base_dir(tests[i].base_dir);
        string data_file = base_dir + "/input.xml";
        string map_file = base_dir + "/map.xml";
        string check_file = base_dir + "/check.txt";

        // Load the data file content.
        cout << "reading " << data_file << endl;
        load_file_content(data_file.c_str(), strm);

        boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
        boost::scoped_ptr<spreadsheet::import_factory> import_fact(new spreadsheet::import_factory(doc.get()));
        boost::scoped_ptr<spreadsheet::export_factory> export_fact(new spreadsheet::export_factory(doc.get()));

        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();

        // Parse the map file to define map rules, and parse the data file.
        orcus_xml app(repo, import_fact.get(), export_fact.get());
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

        // Output to xml file with the linked values coming from the document.
        string out_file = temp_output_xml;
        cout << "writing to " << out_file << endl;
        app.write_file(out_file.c_str());

        if (tests[i].output_equals_input)
        {
            // Compare the logical xml content of the output xml with the
            // input one. They should be identical.

            string dump_input, dump_output;
            string strm_data_file, strm_out_file; // Hold the stream content in memory while the namespace context is being used.
            dump_xml_structure(dump_input, strm_data_file, data_file.c_str(), cxt);
            dump_xml_structure(dump_output, strm_out_file, out_file.c_str(), cxt);
            assert(!dump_input.empty() && !dump_output.empty());
            assert(dump_input == dump_output);
        }

        // Delete the temporary xml output.
        unlink(out_file.c_str());
    }
}

} // anonymous namespace

int main()
{
    test_mapped_xml_import();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
