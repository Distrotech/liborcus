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

#include <cstdlib>
#include <cassert>
#include <iostream>

using namespace orcus;
using namespace std;

void test_path_insertion()
{
    xml_map_tree tree;
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 1;

    // Single cell links
    tree.set_cell_link("/data/elem1", ref);
    const xml_map_tree::element* p = tree.get_link("/data/elem1");
    assert(p && p->type == xml_map_tree::element_cell_ref);
    assert(p->cell_ref->pos.sheet == "test");
    assert(p->cell_ref->pos.row == 2);
    assert(p->cell_ref->pos.col == 1);

    const xml_map_tree::element* elem1 = p;

    ref.row = 3;
    ref.col = 2;
    tree.set_cell_link("/data/elem2", ref);
    p = tree.get_link("/data/elem2");
    assert(p && p->type == xml_map_tree::element_cell_ref);
    assert(p->cell_ref->pos.sheet == "test");
    assert(p->cell_ref->pos.row == 3);
    assert(p->cell_ref->pos.col == 2);

    // The link in elem1 should be unchanged.
    p = tree.get_link("/data/elem1");
    assert(p == elem1);

    ref.sheet = pstring("test2");
    ref.row = 10;
    ref.col = 5;
    tree.set_cell_link("/data/meta/title", ref);
    p = tree.get_link("/data/meta/title");
    assert(p && p->type == xml_map_tree::element_cell_ref);
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
    p = tree.get_link("/data/entries/entry/id");
    assert(p && p->type == xml_map_tree::element_range_field_ref);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 0);

    p = tree.get_link("/data/entries/entry/name");
    assert(p && p->type == xml_map_tree::element_range_field_ref);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 1);

    p = tree.get_link("/data/entries/entry/score");
    assert(p && p->type == xml_map_tree::element_range_field_ref);
    assert(p->field_ref->ref->pos.sheet == "test3");
    assert(p->field_ref->ref->pos.row == 5);
    assert(p->field_ref->ref->pos.col == 0);
    assert(p->field_ref->column_pos == 2);
}

void test_tree_walk()
{
    xml_map_tree tree;
    xml_map_tree::cell_position ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 1;

    tree.set_cell_link("/data/header/title", ref);
    xml_map_tree::walker walker = tree.get_tree_walker();
    walker.reset();

    // Root element.
    const xml_map_tree::element* elem = walker.push_element("data");
    assert(elem);
    assert(elem->name == "data");
    assert(elem->type == xml_map_tree::element_non_leaf);

    elem = walker.push_element("header");
    assert(elem);
    assert(elem->name == "header");
    assert(elem->type == xml_map_tree::element_non_leaf);

    elem = walker.push_element("title");
    assert(elem);
    assert(elem->name == "title");
    assert(elem->type == xml_map_tree::element_cell_ref);

    elem = walker.pop_element("title");
    assert(elem);
    assert(elem->name == "header");
    assert(elem->type == xml_map_tree::element_non_leaf);

    elem = walker.pop_element("header");
    assert(elem);
    assert(elem->name == "data");
    assert(elem->type == xml_map_tree::element_non_leaf);

    elem = walker.pop_element("data");
    assert(!elem);
}

int main()
{
    test_path_insertion();
    test_tree_walk();
    return EXIT_SUCCESS;
}
