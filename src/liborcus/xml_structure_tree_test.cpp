/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/xml_structure_tree.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;
using namespace orcus;

const char* base_dirs[] = {
    SRCDIR"/test/xml-structure/basic-1/",
    SRCDIR"/test/xml-structure/basic-2/",
    SRCDIR"/test/xml-structure/basic-3/",
    SRCDIR"/test/xml-structure/attribute-1/",
    SRCDIR"/test/xml-structure/nested-repeat-1/",
    SRCDIR"/test/xml-structure/namespace-default/"
};

void test_basic()
{
    size_t n = sizeof(base_dirs)/sizeof(base_dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        string filepath(base_dirs[i]);
        filepath.append("input.xml");
        string strm = load_file_content(filepath.c_str());
        assert(!strm.empty());
        xmlns_repository xmlns_repo;
        xmlns_context cxt = xmlns_repo.create_context();
        xml_structure_tree tree(cxt);
        tree.parse(&strm[0], strm.size());
        ostringstream os;
        tree.dump_compact(os);
        string data_content = os.str();
        cout << "--" << endl;
        cout << data_content;

        // Check the dump content against known datum.
        filepath = base_dirs[i];
        filepath.append("check.txt");
        string strm_check = load_file_content(filepath.c_str());
        assert(!strm_check.empty());

        // They should be identical, plus or minus leading/trailing whitespaces.
        pstring s1(&data_content[0], data_content.size());
        pstring s2(&strm_check[0], strm_check.size());
        assert(s1.trim() == s2.trim());
    }
}

void test_walker()
{
    {
        string filepath(base_dirs[0]);
        filepath.append("input.xml");
        string strm = load_file_content(filepath.c_str());
        assert(!strm.empty());
        xmlns_repository xmlns_repo;
        xmlns_context cxt = xmlns_repo.create_context();
        xml_structure_tree tree(cxt);
        tree.parse(&strm[0], strm.size());

        // Get walker from the tree.
        xml_structure_tree::entity_names_type elem_names;
        xml_structure_tree::walker wkr = tree.get_walker();

        // Root element.
        xml_structure_tree::element elem = wkr.root();
        assert(elem.name.name == "root");
        assert(!elem.repeat);

        // Get names of child elements. There should only one one and it should be 'entry'.
        wkr.get_children(elem_names);
        assert(elem_names.size() == 1);
        xml_structure_tree::entity_name elem_name = elem_names.front();
        assert(elem_name.name == "entry");

        // Descend into 'entry'.
        elem = wkr.descend(elem_name);
        assert(elem.name.name == "entry");
        assert(elem.repeat);

        // Element 'entry' should have 2 child elements 'name' and 'id' in this order.
        wkr.get_children(elem_names);
        assert(elem_names.size() == 2);
        assert(elem_names[0].name == "name");
        assert(elem_names[1].name == "id");

        // Descend into 'name'.
        elem_name = elem_names[0];
        elem = wkr.descend(elem_name);
        assert(elem.name.name == "name");
        assert(!elem.repeat);

        // This is a leaf element. It should have no child elements.
        xml_structure_tree::entity_names_type test_names;
        wkr.get_children(test_names);
        assert(test_names.empty());

        // Move up to 'entry'.
        elem = wkr.ascend();
        assert(elem.name.name == "entry");
        assert(elem.repeat);

        // Move down to 'id'.
        elem = wkr.descend(elem_names[1]);
        assert(elem.name.name == "id");
        assert(!elem.repeat);

        // Move up to 'entry' again.
        elem = wkr.ascend();
        assert(elem.name.name == "entry");
        assert(elem.repeat);

        // Move up to 'root'.
        elem = wkr.ascend();
        assert(elem.name.name == "root");
        assert(!elem.repeat);
    }

    {
        string filepath(base_dirs[3]); // attribute-1
        filepath.append("input.xml");

        string strm = load_file_content(filepath.c_str());
        assert(!strm.empty());
        xmlns_repository xmlns_repo;
        xmlns_context cxt = xmlns_repo.create_context();
        xml_structure_tree tree(cxt);
        tree.parse(&strm[0], strm.size());

        // Get walker from the tree.
        xml_structure_tree::entity_names_type elem_names;
        xml_structure_tree::walker wkr = tree.get_walker();

        // Root element.
        xml_structure_tree::element elem = wkr.root();
        assert(elem.name.name == "root");
        assert(!elem.repeat);

        // Check attributes of root, which should have 'version' and 'type' in this order.
        xml_structure_tree::entity_names_type names;
        wkr.get_attributes(names);
        assert(names.size() == 2);
        assert(names[0].name == "version");
        assert(names[1].name == "type");

        // Root element should have only one child element 'entry'.
        wkr.get_children(names);
        assert(names.size() == 1);
        assert(names[0].name == "entry");
        elem = wkr.descend(names[0]);
        assert(elem.name.name == "entry");
        assert(elem.repeat);

        // The 'entry' element should have 3 attributes 'attr1', 'attr2', and 'attr3'.
        wkr.get_attributes(names);
        assert(names.size() == 3);
        assert(names[0].name == "attr1");
        assert(names[1].name == "attr2");
        assert(names[2].name == "attr3");
    }
}

int main()
{
    test_basic();
    test_walker();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
