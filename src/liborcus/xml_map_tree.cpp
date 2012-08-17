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

xml_map_tree::xpath_error::xpath_error(const string& msg) : general_error(msg) {}

xml_map_tree::cell_reference::cell_reference() :
    row(-1), col(-1) {}

xml_map_tree::cell_reference::cell_reference(const pstring& _sheet, model::row_t _row, model::col_t _col) :
    sheet(_sheet), row(_row), col(_col) {}

xml_map_tree::cell_reference::cell_reference(const cell_reference& r) :
    sheet(r.sheet), row(r.row), col(r.col) {}

xml_map_tree::element::element(const pstring& _name, element_type _type) :
    name(_name), type(_type)
{
    switch (type)
    {
        case element_cell_ref:
            cell_ref = new cell_reference;
        break;
        case element_non_leaf:
            child_elements = new element_list_type;
        break;
        case element_range_field_ref:
            field_ref = new field_in_range;
        break;
        default:
            throw general_error("unexpected element type in the constructor.");
    }
}

xml_map_tree::element::~element()
{
    switch (type)
    {
        case element_cell_ref:
            delete cell_ref;
        break;
        case element_non_leaf:
            delete child_elements;
        break;
        case element_range_field_ref:
            delete field_ref;
        break;
        default:
            throw general_error("unexpected element type in the destructor.");
    }
}

xml_map_tree::xml_map_tree() : m_root(NULL) {}
xml_map_tree::~xml_map_tree()
{
    delete m_root;
}

void xml_map_tree::set_cell_link(const pstring& xpath, const cell_reference& ref)
{
    if (xpath.empty())
        return;

    cout << "cell link: " << xpath << " (ref=" << ref << ")" << endl;
    element* p = get_element(xpath);
}

void xml_map_tree::set_range_field_link(
   const pstring& xpath, const cell_reference& ref, int column_pos)
{
    if (xpath.empty())
        return;

    cout << "range field link: " << xpath << " (ref=" << ref << "; column=" << column_pos << ")" << endl;
    element* p = get_element(xpath);
}

xml_map_tree::element* xml_map_tree::get_element(const pstring& xpath)
{
    assert(!xpath.empty());
    const char* p = xpath.get();
    if (*p != '/')
        throw xpath_error("first character must be '/'.");

    const char* path = ++p;
    size_t len = 0;
    pstring name;
    for (size_t i = 1, n = xpath.size(); i < n; ++i, ++p, ++len)
    {
        if (*p != '/')
            continue;

        // '/' encountered.
        if (!len)
            throw xpath_error("empty element name is not allowed.");

        name = pstring(path, len);
        cout << name << endl;

        // Skip to the next char.
        ++i;
        path = ++p;
        len = 0;
    }

    if (!len)
        throw xpath_error("empty element name is not allowed.");

    name = pstring(path, len);
    cout << name << endl;

    return NULL;
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_reference& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

}

