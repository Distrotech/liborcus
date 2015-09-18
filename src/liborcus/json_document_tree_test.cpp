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

const char* json_test_refs_dirs[] = {
    SRCDIR"/test/json/refs1/",
};

bool string_expected(const json_document_tree::node& node, const char* expected)
{
    if (node.type() != json_node_t::string)
        return false;

    if (node.string_value() == expected)
        return true;

    cerr << "expected='" << expected << "', actual='" << node.string_value() << "'" << endl;
    return false;
}

bool number_expected(const json_document_tree::node& node, double expected)
{
    if (node.type() != json_node_t::number)
        return false;

    if (node.numeric_value() == expected)
        return true;

    cerr << "expected=" << expected << ", actual=" << node.numeric_value() << endl;
    return false;
}

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

void verify_input(json_config& test_config, const char* basedir)
{
    string json_file(basedir);
    json_file += "input.json";
    test_config.input_path = json_file;

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

void test_json_parse()
{
    json_config test_config;

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(json_test_dirs); ++i)
    {
        const char* basedir = json_test_dirs[i];
        verify_input(test_config, basedir);
    }
}

void test_json_resolve_refs()
{
    json_config test_config;
    test_config.resolve_references = true;

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(json_test_refs_dirs); ++i)
    {
        const char* basedir = json_test_refs_dirs[i];
        verify_input(test_config, basedir);
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
            cout << "invalid expression tested: " << invalid_json << endl;
            cout << "error message received: " << e.what() << endl;
        }
    }
}

std::unique_ptr<json_document_tree> get_doc_tree(const char* filepath)
{
    json_config test_config;

    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;

    auto doc = make_unique<json_document_tree>();
    doc->load(strm, test_config);

    return doc;
}

void test_json_traverse_basic1()
{
    const char* filepath = SRCDIR"/test/json/basic1/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    assert(node.type() == json_node_t::array);
    assert(node.child_count() == 3);
    assert(node.child(0).type() == json_node_t::boolean_true);
    assert(node.child(1).type() == json_node_t::boolean_false);
    assert(node.child(2).type() == json_node_t::null);

    // Move to child node and move back.
    json_document_tree::node node2 = node.child(0).parent();
    assert(node.identity() == node2.identity());
}

void test_json_traverse_basic2()
{
    const char* filepath = SRCDIR"/test/json/basic2/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    assert(node.type() == json_node_t::array);
    assert(node.child_count() == 14);

    assert(string_expected(node.child(0), "I am string"));
    assert(string_expected(node.child(1), "me too"));
    assert(string_expected(node.child(2), ""));
    assert(string_expected(node.child(3), "\\"));
    assert(string_expected(node.child(4), "/"));
    assert(string_expected(node.child(5), "\\b"));
    assert(string_expected(node.child(6), "\\f"));
    assert(string_expected(node.child(7), "\\n"));
    assert(string_expected(node.child(8), "\\r"));
    assert(string_expected(node.child(9), "\\t"));
    assert(string_expected(node.child(10), "\"quoted\""));
    assert(string_expected(node.child(11), "http://www.google.com"));
    assert(string_expected(node.child(12), "one \\n two \\n three"));
    assert(string_expected(node.child(13), "front segment 'single quote' and \"double quote\" end segment"));
}

void test_json_traverse_basic3()
{
    const char* filepath = SRCDIR"/test/json/basic3/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    assert(node.type() == json_node_t::array);
    assert(node.child_count() == 9);

    assert(number_expected(node.child(0), 0.0));
    assert(number_expected(node.child(1), 1.0));
    assert(number_expected(node.child(2), 2.0));
    assert(number_expected(node.child(3), 15.0));
    assert(number_expected(node.child(4), 12.34));
    assert(number_expected(node.child(5), -0.12));
    assert(number_expected(node.child(6), 1.2e+22));
    assert(number_expected(node.child(7), 1.11e-7));
    assert(number_expected(node.child(8), 11E2));
}

void test_json_traverse_basic4()
{
    const char* filepath = SRCDIR"/test/json/basic4/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    assert(node.type() == json_node_t::object);
    auto keys = node.keys();
    assert(keys.size() == 3);
    for (auto it = keys.begin(), ite = keys.end(); it != ite; ++it)
    {
        const pstring& key = *it;
        json_document_tree::node child = node.child(key);
        if (key == "int")
            assert(number_expected(child, 12.0));
        else if (key == "float")
            assert(number_expected(child, 0.125));
        else if (key == "string")
            assert(string_expected(child, "blah..."));
        else
            assert(!"unexpected key");
    }
}

void test_json_traverse_nested1()
{
    const char* filepath = SRCDIR"/test/json/nested1/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    uintptr_t root_id = node.identity();

    assert(node.type() == json_node_t::object);
    assert(node.child_count() == 1);

    node = node.child(0);
    assert(node.type() == json_node_t::array);
    assert(node.child_count() == 3);

    assert(number_expected(node.child(0), 1.0));
    assert(number_expected(node.child(1), 2.0));
    assert(number_expected(node.child(2), 3.0));

    node = node.parent();
    assert(node.identity() == root_id);
}

void test_json_traverse_nested2()
{
    const char* filepath = SRCDIR"/test/json/nested2/input.json";
    std::unique_ptr<json_document_tree> doc = get_doc_tree(filepath);
    json_document_tree::node node = doc->get_document_root();

    assert(node.type() == json_node_t::array);
    assert(node.child_count() == 3);

    node = node.child(0);
    assert(node.type() == json_node_t::object);
    assert(number_expected(node.child("value"), 1.0));
    node = node.parent();

    node = node.child(1);
    assert(node.type() == json_node_t::object);
    assert(number_expected(node.child("value"), 2.0));
    node = node.parent();

    node = node.child(2);
    assert(node.type() == json_node_t::object);
    assert(number_expected(node.child("value"), 3.0));
    node = node.parent();
}

int main()
{
    test_json_parse();
    test_json_resolve_refs();
    test_json_parse_invalid();
    test_json_traverse_basic1();
    test_json_traverse_basic2();
    test_json_traverse_basic3();
    test_json_traverse_basic4();
    test_json_traverse_nested1();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
