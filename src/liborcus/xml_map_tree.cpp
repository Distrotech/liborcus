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
    element* p = get_element(xpath, element_cell_ref);
    assert(p);
}

void xml_map_tree::set_range_field_link(
   const pstring& xpath, const cell_reference& ref, int column_pos)
{
    if (xpath.empty())
        return;

    cout << "range field link: " << xpath << " (ref=" << ref << "; column=" << column_pos << ")" << endl;
    element* p = get_element(xpath, element_range_field_ref);
    assert(p);
}

namespace {

class find_by_name : std::unary_function<xml_map_tree::element, bool>
{
    pstring m_name;
public:
    find_by_name(const pstring& name) : m_name(name) {}
    bool operator() (const xml_map_tree::element& e) const
    {
        return m_name == e.name;
    }
};

}

xml_map_tree::element* xml_map_tree::get_element(const pstring& xpath, element_type type)
{
    assert(!xpath.empty());
    const char* p = xpath.get();
    if (*p != '/')
        throw xpath_error("first character must be '/'.");

    // Start from the char after the '/'.
    const char* path = ++p;
    size_t len = 0;
    pstring name;
    size_t start = 1; // starting position

    element* cur_element = m_root;

    // Get the root element first.
    for (size_t i = start, n = xpath.size(); i < n; ++i, ++p, ++len)
    {
        if (*p != '/')
            continue;

        // '/' encountered.
        if (!len)
            throw xpath_error("empty element name is not allowed.");

        name = pstring(path, len);
        cout << name << " (root)" << endl;
        if (m_root)
        {
            // Make sure the root element's names are the same.
            if (m_root->name != name)
                xpath_error("path begins with inconsistent root level name.");
        }
        else
        {
            // First time the root element is encountered.
            m_root = new element(m_names.intern(path, len), element_non_leaf);
        }

        // Skip to the next char.
        start = ++i;
        path = ++p;
        len = 0;
        break;
    }

    if (!m_root)
    {
        // This means the xpath consists of just one level i.e. '/root'.
        // Should we support this?
        throw xpath_error("path must consist of at least two levels.");
    }

    cur_element = m_root;

    assert(cur_element);
    assert(cur_element->child_elements);

    for (size_t i = start, n = xpath.size(); i < n; ++i, ++p, ++len)
    {
        if (*p != '/')
            continue;

        // '/' encountered.
        if (!len)
            throw xpath_error("empty element name is not allowed.");

        // Insert a non-leaf element.
        name = pstring(path, len);

        // Check if the current element contains a chile element of the same name.
        element_list_type& children = *cur_element->child_elements;
        element_list_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(name));
        if (it == children.end())
        {
            // Insert a new element of this name.
            children.push_back(new element(name, element_non_leaf));
            cur_element = &children.back();
            cout << name << " (new)" << endl;
        }
        else
        {
            cur_element = &(*it);
            cout << name << endl;
        }

        // Skip to the next char.
        ++i;
        path = ++p;
        len = 0;
    }

    if (!len)
        throw xpath_error("empty element name is not allowed.");

    assert(cur_element);

    // Insert a leaf node.
    name = pstring(path, len);
    cout << name << " (leaf)" << endl;

    // Check if an element of the same name already exists.
    element_list_type& children = *cur_element->child_elements;
    element_list_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(name));
    if (it != children.end())
        throw xpath_error("This path is already linked.  You can't link the same path twice.");

    children.push_back(new element(name, type));
    return &children.back();
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_reference& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

}

