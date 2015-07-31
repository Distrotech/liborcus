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

#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace orcus;

string get_stream(const char* path)
{
    cout << path << endl;
    string strm;
    load_file_content(path, strm);
    cout << strm << endl;

    return strm;
}

json_config test_config;

void test_json_parse_basic1()
{
    const char* path = SRCDIR"/test/json/basic1.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_basic2()
{
    const char* path = SRCDIR"/test/json/basic2.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_basic3()
{
    const char* path = SRCDIR"/test/json/basic3.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_basic4()
{
    const char* path = SRCDIR"/test/json/basic4.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_nested1()
{
    const char* path = SRCDIR"/test/json/nested1.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_nested2()
{
    const char* path = SRCDIR"/test/json/nested2.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm, test_config);
    cout << doc.dump() << endl;
}

void test_json_parse_invalid()
{
    const char* invalids[] = {
        "[foo]",
        "[qwerty]",
        "[1,2] null",
        "{\"key\" 1: 12}",
        "[1,,2]"
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
    test_json_parse_basic1();
    test_json_parse_basic2();
    test_json_parse_basic3();
    test_json_parse_basic4();
    test_json_parse_nested1();
    test_json_parse_nested2();
    test_json_parse_invalid();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
