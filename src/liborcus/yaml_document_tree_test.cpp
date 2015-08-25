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

void test_yaml_parse()
{
    const char* filepath = SRCDIR"/test/yaml/basic1/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    yaml_document_tree::walker walker = doc.get_walker();
    assert(walker.child_count() == 1);

    // Document root is a map node with 4 elements.
    yaml_document_tree::node root = walker.first_child();
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
}

int main()
{
    test_yaml_parse();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
