/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/stream.hpp"

#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

void test_yaml_parse()
{
    using node_type = yaml_document_tree::walker::node_type;

    const char* filepath = SRCDIR"/test/yaml/basic1/input.yaml";
    cout << filepath << endl;
    string strm = load_file_content(filepath);
    cout << strm << endl;
    yaml_document_tree doc;
    doc.load(strm);

    yaml_document_tree::walker walker = doc.get_walker();
    assert(walker.type() == node_type::document_list);
    assert(walker.child_count() == 1);

    // Document root is a map node with 4 elements.
    walker.first_child();
    assert(walker.type() == node_type::map);
    assert(walker.child_count() == 4);
}

int main()
{
    test_yaml_parse();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
