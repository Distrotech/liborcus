/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/stream.hpp"
#include "orcus/json_document_tree.hpp"

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

void test_json_parse_basic1()
{
    const char* path = SRCDIR"/test/json/basic1.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm);
}

void test_json_parse_basic2()
{
    const char* path = SRCDIR"/test/json/basic2.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm);
}

void test_json_parse_basic3()
{
    const char* path = SRCDIR"/test/json/basic3.json";
    string strm = get_stream(path);
    json_document_tree doc;
    doc.load(strm);
}

int main()
{
    test_json_parse_basic1();
    test_json_parse_basic2();
    test_json_parse_basic3();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
