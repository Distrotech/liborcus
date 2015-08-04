/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/stream.hpp"
#include "orcus/json_document_tree.hpp"
#include "orcus/json_parser_base.hpp"
#include "orcus/global.hpp"
#include "orcus/config.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/dom_tree.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace orcus;

const char* json_test_dirs[] = {
    SRCDIR"/test/json/basic1/",
    SRCDIR"/test/json/basic2/",
    SRCDIR"/test/json/basic3/",
    SRCDIR"/test/json/basic4/",
    SRCDIR"/test/json/nested1/",
    SRCDIR"/test/json/nested2/",
    SRCDIR"/test/json/swagger/"
};

string dump_check_content(const json_document_tree& doc)
{
    string xml_strm = doc.dump_xml();
    xmlns_repository repo;
    xmlns_context cxt = repo.create_context();
    dom_tree dom(cxt);
    dom.load(xml_strm);

    ostringstream os;
    dom.dump_compact(os);
    return os.str();
}

bool compare_check_contents(const std::string& expected, const std::string& actual)
{
    pstring _expected(expected.data(), expected.size());
    pstring _actual(actual.data(), actual.size());
    _expected = _expected.trim();
    _actual = _actual.trim();

    return _expected == _actual;
}

void test_json_parse()
{
    json_config test_config;

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(json_test_dirs); ++i)
    {
        const char* basedir = json_test_dirs[i];
        string json_file(basedir);
        json_file += "input.json";

        cout << "Testing " << json_file << endl;

        string strm = load_file_content(json_file.c_str());
        json_document_tree doc;
        doc.load(strm, test_config);

        string check_file(basedir);
        check_file += "check.txt";
        string check_master = load_file_content(check_file.c_str());
        string check_doc = dump_check_content(doc);

        assert(compare_check_contents(check_master, check_doc));
    }
}

void test_json_parse_invalid()
{
    json_config test_config;

    const char* invalids[] = {
        "[foo]",
        "[qwerty]",
        "[1,2] null",
        "{\"key\" 1: 12}",
        "[1,,2]",
        "\"key\": {\"inner\": 12}"
    };

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(invalids); ++i)
    {
        const char* invalid_json = invalids[i];
        json_document_tree doc;
        try
        {
            doc.load(string(invalid_json, strlen(invalid_json)), test_config);
            cerr << "Invalid JSON expression is parsed as valid: '" << invalid_json << "'" << endl;
            assert(false);
        }
        catch (const json::parse_error& e)
        {
            // works as expected.
            cout << "expression: " << invalid_json << endl;
            cout << "error: " << e.what() << endl;
        }
    }
}

int main()
{
    test_json_parse();
    test_json_parse_invalid();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
