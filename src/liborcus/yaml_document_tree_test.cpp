/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/stream.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

bool string_expected(const yaml_document_tree::node& node, const char* expected)
{
    if (node.type() != yaml_node_t::string)
        return false;

    if (node.string_value() == expected)
        return true;

    cerr << "expected='" << expected << "', actual='" << node.string_value() << "'" << endl;
    return false;
}

bool number_expected(const yaml_document_tree::node& node, double expected)
{
    if (node.type() != yaml_node_t::number)
        return false;
    return node.numeric_value() == expected;
}

void test_yaml_parse_basic1()
{
    const char* filepath = SRCDIR"/test/yaml/basic1/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);

    // Document root is a map node with 4 elements.
    yaml_document_tree::node root = doc.get_document_root(0);
    assert(root.type() == yaml_node_t::map);
    assert(root.child_count() == 4);

    yaml_document_tree::node key = root.key(0);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "dict");

    key = root.key(1);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "list");

    key = root.key(2);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "number");

    key = root.key(3);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "string");

    // first child is a map.
    yaml_document_tree::node node = root.child(0);
    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    key = node.key(0);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "a");

    key = node.key(1);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "b");

    key = node.key(2);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "c");

    node = node.child(0);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1.0);
    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 2.0);
    node = node.parent();

    node = node.child(2);
    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 2);
    yaml_document_tree::node child = node.child(0);
    assert(child.type() == yaml_node_t::string);
    assert(child.string_value() == "foo");
    child = node.child(1);
    assert(child.type() == yaml_node_t::string);
    assert(child.string_value() == "bar");

    // Go up to the root node.
    node = node.parent().parent();
    assert(node.type() == yaml_node_t::map);

    node = node.child(1);
    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 3);

    node = node.child(0);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1.0);
    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 2.0);
    node = node.parent();

    node = node.child(2);
    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    key = node.key(0);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "a");

    key = node.key(1);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "b");

    key = node.key(2);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "c");

    node = node.child(0);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1.1);
    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1.2);
    node = node.parent();

    node = node.child(2);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1.3);
    node = node.parent();

    node = node.parent().parent();  // back to the root.

    key = node.key(2);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "number");

    key = node.key(3);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "string");

    node = node.child(2);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 12.3);
    node = node.parent();

    node = node.child(3);
    assert(node.type() == yaml_node_t::string);
    assert(node.string_value() == "foo");
    node = node.parent();
}

void test_yaml_parse_basic2()
{
    const char* filepath = SRCDIR"/test/yaml/basic2/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 3);

    node = node.child(0);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 1);
    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 2);
    node = node.parent();

    node = node.child(2);
    assert(node.type() == yaml_node_t::number);
    assert(node.numeric_value() == 3);
    node = node.parent();
}

void test_yaml_parse_basic3()
{
    const char* filepath = SRCDIR"/test/yaml/basic3/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 2);

    node = node.child(0);
    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    assert(string_expected(node.key(0), "a"));
    assert(string_expected(node.key(1), "b"));
    assert(string_expected(node.key(2), "c"));

    assert(number_expected(node.child(0), 1));
    assert(number_expected(node.child(1), 2));
    assert(number_expected(node.child(2), 3));

    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    assert(string_expected(node.key(0), "d"));
    assert(string_expected(node.key(1), "e"));
    assert(string_expected(node.key(2), "f"));

    assert(number_expected(node.child(0), 4));
    assert(number_expected(node.child(1), 5));
    assert(number_expected(node.child(2), 6));
}

void test_yaml_parse_null()
{
    const char* filepath = SRCDIR"/test/yaml/null/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 6);

    node = node.child(0);
    assert(node.type() == yaml_node_t::null);
    node = node.parent();

    node = node.child(1);
    assert(node.type() == yaml_node_t::null);
    node = node.parent();

    node = node.child(2);
    assert(node.type() == yaml_node_t::null);
    node = node.parent();

    node = node.child(3);
    assert(node.type() == yaml_node_t::null);
    node = node.parent();

    node = node.child(4);
    assert(node.type() == yaml_node_t::string);
    assert(node.string_value() == "nULL");
    node = node.parent();

    node = node.child(5);
    assert(node.type() == yaml_node_t::string);
    assert(node.string_value() == "NUll");
    node = node.parent();
}

void test_yaml_parse_boolean()
{
    const char* filepath = SRCDIR"/test/yaml/boolean/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    yaml_document_tree::node key = node.key(0);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "positive");

    key = node.key(1);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "negative");

    key = node.key(2);
    assert(key.type() == yaml_node_t::string);
    assert(key.string_value() == "non boolean");

    // list of boolean true's.
    node = node.child(0);
    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 11);
    for (size_t i = 0; i < node.child_count(); ++i)
    {
        yaml_document_tree::node child = node.child(i);
        assert(child.type() == yaml_node_t::boolean_true);
    }
    node = node.parent();

    // list of boolean false's.
    node = node.child(1);
    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 11);
    for (size_t i = 0; i < node.child_count(); ++i)
    {
        yaml_document_tree::node child = node.child(i);
        assert(child.type() == yaml_node_t::boolean_false);
    }
    node = node.parent();

    // list of strings.
    const char* values[] = {
        "yES",
        "nO",
        "tRUE",
        "faLSE",
        "oN",
        "oFF"
    };

    node = node.child(2);
    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == ORCUS_N_ELEMENTS(values));

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(values); ++i)
    {
        node = node.child(i);
        assert(node.type() == yaml_node_t::string);
        assert(node.string_value() == values[i]);
        node = node.parent();
    }
}

void test_yaml_parse_quoted_string()
{
    const char* filepath = SRCDIR"/test/yaml/quoted-string/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 3);

    assert(string_expected(node.key(0), "I am quoted: ~ "));
    assert(string_expected(node.key(1), "list with quoted string values"));
    assert(string_expected(node.key(2), "single quoted string values"));
    assert(string_expected(node.child(0), "Here is another quote."));

    node = node.child(1);

    {
        // list of strings.
        const char* values[] = {
            "1 2 3",
            "null",
            "true",
            "false",
        };

        size_t n = ORCUS_N_ELEMENTS(values);
        assert(node.type() == yaml_node_t::sequence);
        assert(node.child_count() == n);

        for (size_t i = 0; i < n; ++i)
            assert(string_expected(node.child(i), values[i]));
    }

    node = node.parent().child(2);

    {
        // list of strings.
        const char* values[] = {
            "8.8.8.8",
            "'single quote inside'",
            "prefix 'quoted' suffix",
            "\"double quote\"",
            "before \"quote\" after",
            "http://www.google.com",
            "'''",
            " ' ' ' "
        };

        size_t n = ORCUS_N_ELEMENTS(values);
        assert(node.type() == yaml_node_t::sequence);
        assert(node.child_count() == n);

        for (size_t i = 0; i < n; ++i)
            assert(string_expected(node.child(i), values[i]));
    }
}

void test_yaml_parse_multi_line_1()
{
    const char* filepath = SRCDIR"/test/yaml/multi-line-1/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);
    assert(string_expected(node, "1 2 3"));
    assert(node.child_count() == 0);
}

void test_yaml_parse_multi_line_2()
{
    const char* filepath = SRCDIR"/test/yaml/multi-line-2/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);
    assert(string_expected(node, "1 - 2 - 3"));
    assert(node.child_count() == 0);
}

void test_yaml_parse_literal_block_1()
{
    const char* filepath = SRCDIR"/test/yaml/literal-block-1/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(string_expected(node, "line 1\n  line 2\nline 3\n2 blanks follow  "));
    assert(node.child_count() == 0);
}

void test_yaml_parse_literal_block_2()
{
    const char* filepath = SRCDIR"/test/yaml/literal-block-2/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 2);

    assert(string_expected(node.key(0),   "literal block"));
    assert(string_expected(node.child(0), "line 1\n line 2\n  line 3"));
    assert(string_expected(node.key(1),   "multi line"));
    assert(string_expected(node.child(1), "line 1 line 2 line 3"));
}

void test_yaml_parse_url()
{
    const char* filepath = SRCDIR"/test/yaml/url/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    assert(doc.get_document_count() == 1);
    yaml_document_tree::node node = doc.get_document_root(0);

    assert(node.type() == yaml_node_t::sequence);
    assert(node.child_count() == 3);

    assert(string_expected(node.child(0), "http://www.google.com/"));
    assert(string_expected(node.child(1), "mailto:joe@joe-me.com"));

    node = node.child(2);
    assert(node.type() == yaml_node_t::map);
    assert(node.child_count() == 2);

    assert(string_expected(node.key(0), "orcus-url"));
    assert(string_expected(node.key(1), "debian-bugs"));
    assert(string_expected(node.child(0), "http://gitlab.com/orcus/orcus"));
    assert(string_expected(node.child(1), "mailto:submit@bugs.debian.org"));
}

int main()
{
    test_yaml_parse_basic1();
    test_yaml_parse_basic2();
    test_yaml_parse_basic3();
    test_yaml_parse_null();
    test_yaml_parse_boolean();
    test_yaml_parse_quoted_string();
    test_yaml_parse_multi_line_1();
    test_yaml_parse_multi_line_2();
    test_yaml_parse_literal_block_1();
    test_yaml_parse_literal_block_2();
    test_yaml_parse_url();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

