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
#include "orcus/global.hpp"

#define ORCUS_DEBUG_XML 1

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

using namespace std;

namespace orcus {

namespace {

class find_by_name : std::unary_function<xml_map_tree::element, bool>
{
    xmlns_id_t m_ns;
    pstring m_name;
public:
    find_by_name(xmlns_id_t ns, const pstring& name) : m_ns(ns), m_name(name) {}
    bool operator() (const xml_map_tree::element& e) const
    {
        return m_ns == e.ns && m_name == e.name;
    }
};

class xpath_parser
{
    const char* mp_char;
    const char* mp_end;
public:
    xpath_parser(const char* p, size_t n) : mp_char(p), mp_end(p+n)
    {
        if (!n)
            throw xml_map_tree::xpath_error("empty path");

        if (*p != '/')
            throw xml_map_tree::xpath_error("first character must be '/'.");

        ++mp_char;
    }

    pstring next()
    {
        if (mp_char == mp_end)
            return pstring();

        const char* p0 = mp_char;
        size_t len = 0;
        for (;mp_char != mp_end; ++mp_char, ++len)
        {
            if (*mp_char != '/')
                continue;

            // '/' encountered.
            ++mp_char; // skip the '/'.
            return pstring(p0, len);
        }

        // '/' has never been encountered.  It must be the last name in the path.
        return pstring(p0, len);
    }
};

}

xml_map_tree::xpath_error::xpath_error(const string& msg) : general_error(msg) {}

xml_map_tree::cell_position::cell_position() :
    row(-1), col(-1) {}

xml_map_tree::cell_position::cell_position(const pstring& _sheet, spreadsheet::row_t _row, spreadsheet::col_t _col) :
    sheet(_sheet), row(_row), col(_col) {}

xml_map_tree::cell_position::cell_position(const cell_position& r) :
    sheet(r.sheet), row(r.row), col(r.col) {}

xml_map_tree::cell_reference::cell_reference() :
    element_open_begin(NULL), element_open_end(NULL), element_close_begin(NULL), element_close_end(NULL) {}

xml_map_tree::range_reference::range_reference(const cell_position& _pos) :
    pos(_pos), row_size(0),
    element_open_begin(NULL), element_open_end(NULL), element_close_begin(NULL), element_close_end(NULL) {}

xml_map_tree::linkable::linkable(xmlns_id_t _ns, const pstring& _name, bool _attribute) :
    ns(_ns), name(_name), attribute(_attribute) {}

xml_map_tree::attribute::attribute(xmlns_id_t _ns, const pstring& _name, attribute_type _type) :
    linkable(_ns, _name, true), type(_type)
{
    switch (type)
    {
        case attribute_cell_ref:
            cell_ref = new cell_reference;
        break;
        case attribute_range_field_ref:
            field_ref = new field_in_range;
        break;
        default:
            throw general_error("unexpected attribute type in the constructor");
    }
}

xml_map_tree::attribute::~attribute()
{
    switch (type)
    {
        case attribute_cell_ref:
            delete cell_ref;
        break;
        case attribute_range_field_ref:
            delete field_ref;
        break;
        default:
            throw general_error("unexpected attribute type in the destructor.");
    }
}

xml_map_tree::element::element(xmlns_id_t _ns, const pstring& _name, element_type _type) :
    linkable(_ns, _name, false), type(_type), range_parent(NULL)
{
    switch (type)
    {
        case element_cell_ref:
            cell_ref = new cell_reference;
        break;
        case element_non_leaf:
            child_elements = new element_store_type;
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

const xml_map_tree::element* xml_map_tree::element::get_child(xmlns_id_t _ns, const pstring& _name) const
{
    if (type != element_non_leaf)
        return NULL;

    assert(child_elements);

    element_store_type::const_iterator it =
        std::find_if(child_elements->begin(), child_elements->end(), find_by_name(_ns, _name));

    return it == child_elements->end() ? NULL : &(*it);
}

xml_map_tree::walker::walker(const xml_map_tree& parent) :
    m_parent(parent), m_content_depth(0) {}
xml_map_tree::walker::walker(const xml_map_tree::walker& r) :
    m_parent(r.m_parent), m_stack(r.m_stack), m_content_depth(r.m_content_depth) {}

void xml_map_tree::walker::reset()
{
    m_stack.clear();
    m_content_depth = 0;
}

const xml_map_tree::element* xml_map_tree::walker::push_element(xmlns_id_t ns, const pstring& name)
{
    if (m_stack.empty())
    {
        if (!m_parent.mp_root)
            // Tree is empty.
            return NULL;

        const element* p = m_parent.mp_root;
        if (p->ns != ns || p->name != name)
            // Names differ.
            return NULL;

        m_stack.push_back(p);
        return p;
    }

    if (m_stack.back()->type == element_non_leaf)
    {
        // Check if the current element has a child of the same name.
        const element* p = m_stack.back()->get_child(ns, name);
        if (p)
        {
            m_stack.push_back(p);
            return p;
        }

        // The path matched up to this point, but no more.
        ++m_content_depth;
        return NULL;
    }

    // Current element is linked.
    assert(m_stack.back()->type != element_non_leaf);
    ++m_content_depth;
    return m_stack.back();
}

const xml_map_tree::element* xml_map_tree::walker::pop_element(xmlns_id_t ns, const pstring& name)
{
    if (m_content_depth)
    {
        --m_content_depth;
        return m_stack.back();
    }

    if (m_stack.empty())
        throw general_error("Element was popped while the stack was empty.");

    if (m_stack.back()->ns != ns || m_stack.back()->name != name)
        throw general_error("Closing element has a different name than the opening element.");

    m_stack.pop_back();
    return m_stack.empty() ? NULL : m_stack.back();
}

xml_map_tree::xml_map_tree() : mp_cur_range_ref(NULL), mp_root(NULL) {}
xml_map_tree::~xml_map_tree()
{
    std::for_each(m_field_refs.begin(), m_field_refs.end(), map_object_deleter<range_ref_map_type>());
    delete mp_root;
}

void xml_map_tree::set_cell_link(const pstring& xpath, const cell_position& ref)
{
    if (xpath.empty())
        return;

    cout << "cell link: " << xpath << " (ref=" << ref << ")" << endl;
    element_list_type elem_stack;
    get_element_stack(xpath, element_cell_ref, elem_stack);
    assert(!elem_stack.empty());
    element* p = elem_stack.back();
    assert(p && p->cell_ref);
    p->cell_ref->pos = ref;

    // Make sure the sheet name string is persistent.
    p->cell_ref->pos.sheet = m_names.intern(ref.sheet.get(), ref.sheet.size());
}

void xml_map_tree::start_range()
{
    m_cur_range_parent.clear();
    mp_cur_range_ref = NULL;
}

void xml_map_tree::append_range_field_link(const pstring& xpath, const cell_position& pos)
{
    if (xpath.empty())
        return;

    range_reference* range_ref = NULL;
    range_ref_map_type::iterator it = m_field_refs.lower_bound(pos);
    if (it == m_field_refs.end() || m_field_refs.key_comp()(pos, it->first))
    {
        // This reference does not exist yet.  Insert a new one.

        // Make sure the sheet name string is persistent.
        cell_position pos_safe = pos;
        pos_safe.sheet = m_names.intern(pos.sheet.get(), pos.sheet.size());

        it = m_field_refs.insert(it, range_ref_map_type::value_type(pos_safe, new range_reference(pos_safe)));
    }

    range_ref = it->second;
    assert(range_ref);

    if (!mp_cur_range_ref)
        mp_cur_range_ref = range_ref;

    cout << "range field link: " << xpath << " (ref=" << pos << ")" << endl;
    element_list_type elem_stack;
    get_element_stack(xpath, element_range_field_ref, elem_stack);
    if (elem_stack.size() <= 3)
        throw xpath_error("Path of a range field link must be at least 3 levels.");

    element* p = elem_stack.back();
    assert(p && p->field_ref);
    p->field_ref->ref = range_ref;
    p->field_ref->column_pos = range_ref->elements.size();

    range_ref->elements.push_back(p);

    // Determine the deepest common element for all field link elements in the
    // current range reference.
    if (m_cur_range_parent.empty())
    {
        // First field link in this range.
        element_list_type::iterator it_end = elem_stack.end();
        --it_end; // Skip the leaf element, which is used as a field in a range.
        --it_end; // Skip the next-up element, which is used to group a single record entry.
        m_cur_range_parent.assign(elem_stack.begin(), it_end);
    }
    else
    {
        // Determine the deepest common element between the two.
        element_list_type::iterator it = elem_stack.begin(), it_end = elem_stack.end();
        element_list_type::iterator it_cur = m_cur_range_parent.begin(), it_cur_end = m_cur_range_parent.end();
        if (*it != *it_cur)
            throw xpath_error("Two field links in the same range reference start with different root elements.");

        ++it;
        ++it_cur;

        for (; it != it_end && it_cur != it_cur_end; ++it, ++it_cur)
        {
            if (*it == *it_cur)
                continue;

            // The two elements differ.  Take their parent element as the new common element.
            m_cur_range_parent.assign(elem_stack.begin(), it); // current elemnt excluded.
            break;
        }

        if (m_cur_range_parent.size() <= 1)
            throw xpath_error("Two field links in the same range reference must at least share the first level of their paths.");
    }
}

void xml_map_tree::commit_range()
{
    if (!mp_cur_range_ref)
        // Nothing to commit.
        return;

#if ORCUS_DEBUG_XML
    cout << "parent element path for this range: ";
    element_list_type::iterator it = m_cur_range_parent.begin(), it_end = m_cur_range_parent.end();
    for (; it != it_end; ++it)
        cout << "/" << (**it).name;
    cout << endl;
#endif

    assert(m_cur_range_parent.size() >= 2);
    // Mark the range parent element.
    m_cur_range_parent.back()->range_parent = mp_cur_range_ref;
}

const xml_map_tree::element* xml_map_tree::get_link(const pstring& xpath) const
{
    if (!mp_root)
        return NULL;

    if (xpath.empty())
        return NULL;

    const element* cur_element = mp_root;

    xpath_parser parser(xpath.get(), xpath.size());

    // Check the root element first.
    pstring name = parser.next();
    if (cur_element->name != name)
        return NULL;

    for (name = parser.next(); !name.empty(); name = parser.next())
    {
        // See if an element of this name exists below the current element.
        if (cur_element->type != element_non_leaf)
            return NULL;

        if (!cur_element->child_elements)
            return NULL;

        element_store_type::const_iterator it =
            std::find_if(cur_element->child_elements->begin(), cur_element->child_elements->end(), find_by_name(XMLNS_UNKNOWN_ID, name));
        if (it == cur_element->child_elements->end())
            // No such child element exists.
            return NULL;

        cur_element = &(*it);
    }

    if (cur_element->type == element_non_leaf)
        // Non-leaf elements are not links.
        return NULL;

    return cur_element;
}

xml_map_tree::walker xml_map_tree::get_tree_walker() const
{
    return walker(*this);
}

xml_map_tree::range_ref_map_type& xml_map_tree::get_range_references()
{
    return m_field_refs;
}

void xml_map_tree::get_element_stack(const pstring& xpath, element_type type, element_list_type& elem_stack)
{
    assert(!xpath.empty());
    xpath_parser parser(xpath.get(), xpath.size());

    element_list_type elem_stack_new;

    // Get the root element first.
    pstring name = parser.next();
    if (mp_root)
    {
        // Make sure the root element's names are the same.
        if (mp_root->name != name)
            throw xpath_error("path begins with inconsistent root level name.");
    }
    else
    {
        // First time the root element is encountered.
        mp_root = new element(XMLNS_UNKNOWN_ID, m_names.intern(name.get(), name.size()), element_non_leaf);
    }

    elem_stack_new.push_back(mp_root);
    element* cur_element = elem_stack_new.back();
    assert(cur_element);
    assert(cur_element->child_elements);

    name = parser.next();
    for (pstring name_next = parser.next();!name_next.empty(); name_next = parser.next())
    {
        // Check if the current element contains a chile element of the same name.
        element_store_type& children = *cur_element->child_elements;
        element_store_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(XMLNS_UNKNOWN_ID, name));
        if (it == children.end())
        {
            // Insert a new element of this name.
            children.push_back(new element(XMLNS_UNKNOWN_ID, m_names.intern(name.get(), name.size()), element_non_leaf));
            cur_element = &children.back();
        }
        else
            cur_element = &(*it);

        elem_stack_new.push_back(cur_element);
        name = name_next;
    }

    assert(cur_element);

    // Insert a leaf node.

    // Check if an element of the same name already exists.
    element_store_type& children = *cur_element->child_elements;
    element_store_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(XMLNS_UNKNOWN_ID, name));
    if (it != children.end())
        throw xpath_error("This path is already linked.  You can't link the same path twice.");

    children.push_back(new element(XMLNS_UNKNOWN_ID, m_names.intern(name.get(), name.size()), type));
    elem_stack_new.push_back(&children.back());

    elem_stack.swap(elem_stack_new);
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_position& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

bool operator< (const xml_map_tree::cell_position& left, const xml_map_tree::cell_position& right)
{
    if (left.sheet != right.sheet)
        return left.sheet < right.sheet;

    if (left.row != right.row)
        return left.row < right.row;

    return left.col < right.col;
}

}

