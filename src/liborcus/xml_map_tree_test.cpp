/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "xml_map_tree.hpp"
#include "orcus/xml_namespace.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

void test_path_insertion()
{
    xmlns_repository repo;
    xml_map_tree tree(repo);
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 1;

    // Single cell links
    tree.set_cell_link("/data/elem1", ref);
    const xml_map_tree::linkable* p0 = tree.get_link("/data/elem1");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    const xml_map_tree::element* p = static_cast<const xml_map_tree::element*>(p0);
    assert(p->ref_type == xml_map_tree::reference_cell);
    assert(p->cell_ref->pos.sheet == "test");
    assert(p->cell_ref->pos.row == 2);
    assert(p->cell_ref->pos.col == 1);

    const xml_map_tree::element* elem1 = p;

    ref.row = 3;
    ref.col = 2;
    tree.set_cell_link("/data/elem2", ref);
    p0 = tree.get_link("/data/elem2");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    p = static_cast<const xml_map_tree::element*>(p0);
    assert(p && p->ref_type == xml_map_tree::reference_cell);
    assert(p->cell_ref->pos.sheet == "test");
    assert(p->cell_ref->pos.row == 3);
    assert(p->cell_ref->pos.col == 2);

    // The link in elem1 should be unchanged.
    p0 = tree.get_link("/data/elem1");
    assert(p0 == elem1);

    ref.sheet = pstring("test2");
    ref.row = 10;
    ref.col = 5;
    tree.set_cell_link("/data/meta/title", ref);
    p0 = tree.get_link("/data/meta/title");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    p = static_cast<const xml_map_tree::element*>(p0);
    assert(p && p->ref_type == xml_map_tree::reference_cell);
    assert(p->cell_ref->pos.sheet == "test2");
    assert(p->cell_ref->pos.row == 10);
    assert(p->cell_ref->pos.col == 5);

    // Range field links
    ref.row = 5;
    ref.col = 0;
    ref.sheet = pstring("test3");
    tree.append_range_field_link("/data/entries/entry/id", ref);
    tree.append_range_field_link("/data/entries/entry/name", ref);
    tree.append_range_field_link("/data/entries/entry/score", ref);
    p0 = tree.get_link("/data/entries/entry/id");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    p = static_cast<const xml_map_tree::element*>(p0);
    assert(p && p->ref_type == xml_map_tree::reference_range_field);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 0);

    p0 = tree.get_link("/data/entries/entry/name");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    p = static_cast<const xml_map_tree::element*>(p0);
    assert(p && p->ref_type == xml_map_tree::reference_range_field);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 1);

    p0 = tree.get_link("/data/entries/entry/score");
    assert(p0 && p0->node_type == xml_map_tree::node_element);
    p = static_cast<const xml_map_tree::element*>(p0);
    assert(p && p->ref_type == xml_map_tree::reference_range_field);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 2);
}

void test_attr_path_insertion()
{
    xmlns_repository repo;
    xml_map_tree tree(repo);
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 3;

    // 'attr1' is an attribute of 'elem'.
    tree.set_cell_link("/root/elem@attr1", ref);
    const xml_map_tree::linkable* p = tree.get_link("/root/elem@attr1");
    assert(p && p->node_type == xml_map_tree::node_attribute);
    const xml_map_tree::attribute* attr = static_cast<const xml_map_tree::attribute*>(p);
    assert(attr->ref_type == xml_map_tree::reference_cell);
    assert(attr->cell_ref->pos.sheet == "test");
    assert(attr->cell_ref->pos.row == 2);
    assert(attr->cell_ref->pos.col == 3);

    // Insert another attribute in the same element.
    ref.sheet = pstring("test2");
    ref.row = 11;
    ref.col = 4;
    tree.set_cell_link("/root/elem@attr2", ref);
    p = tree.get_link("/root/elem@attr2");
    assert(p && p->node_type == xml_map_tree::node_attribute);
    attr = static_cast<const xml_map_tree::attribute*>(p);
    assert(attr->ref_type == xml_map_tree::reference_cell);
    assert(attr->cell_ref->pos.sheet == "test2");
    assert(attr->cell_ref->pos.row == 11);
    assert(attr->cell_ref->pos.col == 4);

    // At this point, /root/elem is not linked.
    p = tree.get_link("/root/elem");
    assert(!p);

    // Now, link /root/elem.
    ref.sheet = pstring("test3");
    ref.row = 4;
    ref.col = 6;
    tree.set_cell_link("/root/elem", ref);
    p = tree.get_link("/root/elem");
    assert(p && p->node_type == xml_map_tree::node_element);
    const xml_map_tree::element* elem = static_cast<const xml_map_tree::element*>(p);
    assert(elem->elem_type == xml_map_tree::element_linked);
    assert(elem->ref_type == xml_map_tree::reference_cell);
    assert(elem->cell_ref->pos.sheet == "test3");
    assert(elem->cell_ref->pos.row == 4);
    assert(elem->cell_ref->pos.col == 6);
}

void test_tree_walk()
{
    xmlns_repository repo;
    xml_map_tree tree(repo);
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 1;

    tree.set_cell_link("/data/header/title", ref);
    xml_map_tree::walker walker = tree.get_tree_walker();
    walker.reset();

    // Root element.
    const xml_map_tree::element* elem = walker.push_element(XMLNS_UNKNOWN_ID, "data");
    assert(elem);
    assert(elem->name == "data");
    assert(elem->elem_type == xml_map_tree::element_unlinked);

    elem = walker.push_element(XMLNS_UNKNOWN_ID, "header");
    assert(elem);
    assert(elem->name == "header");
    assert(elem->elem_type == xml_map_tree::element_unlinked);

    elem = walker.push_element(XMLNS_UNKNOWN_ID, "title");
    assert(elem);
    assert(elem->name == "title");
    assert(elem->ref_type == xml_map_tree::reference_cell);

    elem = walker.pop_element(XMLNS_UNKNOWN_ID, "title");
    assert(elem);
    assert(elem->name == "header");
    assert(elem->elem_type == xml_map_tree::element_unlinked);

    elem = walker.pop_element(XMLNS_UNKNOWN_ID, "header");
    assert(elem);
    assert(elem->name == "data");
    assert(elem->elem_type == xml_map_tree::element_unlinked);

    elem = walker.pop_element(XMLNS_UNKNOWN_ID, "data");
    assert(!elem);
}

void test_tree_walk_namespace()
{
    xmlns_repository repo;
    xml_map_tree tree(repo);
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("data");
    ref.row = 1;
    ref.col = 2;

    tree.set_namespace_alias("a", "http://some-namespace");
    tree.set_namespace_alias("skip", "http://namespace-to-skip");
    tree.set_cell_link("/a:table/a:title", ref);
    tree.start_range();
    ref.row = 2;
    ref.col = 0;
    tree.append_range_field_link("/a:table/a:rows/a:row/a:city", ref);
    ++ref.col;
    tree.append_range_field_link("/a:table/a:rows/a:row/a:population", ref);
    ++ref.col;
    tree.append_range_field_link("/a:table/a:rows/a:row/a:year", ref);
    tree.commit_range();

    xmlns_id_t ns_a = tree.get_namespace("a");
    assert(ns_a != XMLNS_UNKNOWN_ID);
    xmlns_id_t ns_skip = tree.get_namespace("skip");
    assert(ns_skip != XMLNS_UNKNOWN_ID);

    xml_map_tree::walker walker = tree.get_tree_walker();
    walker.reset();

    // Root element.  This is not linked.
    const xml_map_tree::element* elem = walker.push_element(ns_a, "table");
    assert(elem && elem->ns == ns_a && elem->name == "table" && elem->node_type == xml_map_tree::node_element);
    assert(elem->elem_type == xml_map_tree::element_unlinked);
    assert(elem->ref_type == xml_map_tree::reference_unknown);

    // Intentionally push a foreign element.
    const xml_map_tree::element* elem_old = elem;
    elem = walker.push_element(ns_skip, "foo");
    assert(!elem);
    elem = walker.pop_element(ns_skip, "foo");
    assert(elem == elem_old);

    // Push a foreign element and a valid element under it.  A valid element
    // placed under a foreign element should be invalid.
    elem_old = elem;
    elem = walker.push_element(ns_skip, "foo");
    assert(!elem);
    elem = walker.push_element(ns_a, "title");
    assert(!elem);
    elem = walker.pop_element(ns_a, "title");
    assert(!elem);
    elem = walker.pop_element(ns_skip, "foo");
    assert(elem == elem_old);
}

int main()
{
    test_path_insertion();
    test_attr_path_insertion();
    test_tree_walk();
    test_tree_walk_namespace();
    return EXIT_SUCCESS;
}
