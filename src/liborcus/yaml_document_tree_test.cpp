/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/stream.hpp"
#include "orcus/pstring.hpp"

#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

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

void test_yaml_parse_null()
{
    const char* filepath = SRCDIR"/test/yaml/null/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

}

int main()
{
    test_yaml_parse_basic1();
    test_yaml_parse_basic2();
    test_yaml_parse_null();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
