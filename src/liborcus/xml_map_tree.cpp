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

#define ORCUS_DEBUG_XML 1

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

using namespace std;

namespace orcus {

xml_map_tree::cell_reference::cell_reference() :
    row(-1), col(-1) {}

xml_map_tree::cell_reference::cell_reference(const pstring& _sheet, model::row_t _row, model::col_t _col) :
    sheet(_sheet), row(_row), col(_col) {}

xml_map_tree::cell_reference::cell_reference(const cell_reference& r) :
    sheet(r.sheet), row(r.row), col(r.col) {}

xml_map_tree::element::~element()
{
}

xml_map_tree::xml_map_tree() : m_root(NULL) {}
xml_map_tree::~xml_map_tree()
{
    delete m_root;
}

void xml_map_tree::set_cell_link(const pstring& xpath, const cell_reference& ref)
{
    cout << "cell link: " << xpath << " (ref=" << ref << ")" << endl;
}

void xml_map_tree::set_range_field_link(
   const pstring& xpath, const cell_reference& ref, int column_pos)
{
    cout << "range field link: " << xpath << " (ref=" << ref << "; column=" << column_pos << ")" << endl;
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_reference& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

}
