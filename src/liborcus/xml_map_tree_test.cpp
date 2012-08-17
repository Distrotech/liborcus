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

using namespace orcus;
using namespace std;

void test_path_insertion()
{
    xml_map_tree tree;
    xml_map_tree::cell_reference ref;
    ref.sheet = pstring("test");
    ref.row = 2;
    ref.col = 1;

    tree.set_cell_link("/data/elem1", ref);
    const xml_map_tree::element* p = tree.get_link("/data/elem1");
    assert(p && p->type == xml_map_tree::element_cell_ref);
    assert(p->cell_ref->sheet == "test");
    assert(p->cell_ref->row == 2);
    assert(p->cell_ref->col == 1);
}

int main()
{
    test_path_insertion();
    return EXIT_SUCCESS;
}
