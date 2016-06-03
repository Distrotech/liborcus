/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_map_tree.hpp"
#include "orcus/global.hpp"

#define ORCUS_DEBUG_XML_MAP_TREE 0

#if ORCUS_DEBUG_XML_MAP_TREE
#include <iostream>
#endif

#include <cassert>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

template<typename T>
class find_by_name : std::unary_function<std::unique_ptr<T>, bool>
{
    xmlns_id_t m_ns;
    pstring m_name;
public:
    find_by_name(xmlns_id_t ns, const pstring& name) : m_ns(ns), m_name(name) {}
    bool operator() (const std::unique_ptr<T>& e) const
    {
        return m_ns == e->ns && m_name == e->name;
    }
};

class xpath_parser
{
    const xmlns_context& m_cxt;
    const char* mp_char;
    const char* mp_end;

    enum class token_type { element, attribute };
    token_type m_next_token_type;

public:

    struct token
    {
        xmlns_id_t ns;
        pstring name;
        bool attribute;

        token(xmlns_id_t _ns, const pstring& _name, bool _attribute) :
            ns(_ns), name(_name), attribute(_attribute)
        {
#if ORCUS_DEBUG_XML_MAP_TREE
            cout << "xpath_parser::token: (ns='" << (ns ? ns : "none") << "', name='" << name << "', attribute=" << attribute << ")" << endl;
#endif
        }

        token() : ns(XMLNS_UNKNOWN_ID), attribute(false) {}
        token(const token& r) : ns(r.ns), name(r.name), attribute(r.attribute) {}
    };

    xpath_parser(const xmlns_context& cxt, const char* p, size_t n) :
        m_cxt(cxt), mp_char(p), mp_end(p+n), m_next_token_type(token_type::element)
    {
        if (!n)
            throw xml_map_tree::xpath_error("empty path");

        if (*p != '/')
            throw xml_map_tree::xpath_error("first character must be '/'.");

        ++mp_char;
    }

    token next()
    {
        if (mp_char == mp_end)
            return token();

        const char* p0 = nullptr;
        size_t len = 0;
        xmlns_id_t ns = XMLNS_UNKNOWN_ID;

        for (; mp_char != mp_end; ++mp_char, ++len)
        {
            if (!p0)
            {
                p0 = mp_char;
                len = 0;
            }

            switch (*mp_char)
            {
                case '/':
                {
                    // '/' encountered.  Next token is an element name.
                    if (m_next_token_type == token_type::attribute)
                        throw xml_map_tree::xpath_error("attribute name should not contain '/'.");

                    m_next_token_type = token_type::element;
                    ++mp_char; // skip the '/'.
                    return token(ns, pstring(p0, len), false);
                }
                case '@':
                {
                    // '@' encountered.  Next token is an attribute name.
                    m_next_token_type = token_type::attribute;
                    ++mp_char; // skip the '@'.
                    return token(ns, pstring(p0, len), false);
                }
                case ':':
                {
                    // What comes ':' is a namespace. Reset the name and
                    // convert the namespace to a proper ID.
                    pstring ns_name(p0, len);
                    ns = m_cxt.get(ns_name);
                    p0 = nullptr; // reset the name.
                }
                break;
                default:
                    ;
            }
        }

        // '/' has never been encountered.  It must be the last name in the path.
        return token(ns, pstring(p0, len), m_next_token_type == token_type::attribute);
    }
};

template<typename T>
void print_element_stack(ostream& os, const T& elem_stack)
{
    typename T::const_iterator it = elem_stack.begin(), it_end = elem_stack.end();
    for (; it != it_end; ++it)
    {
        const xml_map_tree::element& elem = **it;
        os << '/' << elem.name;
    }
}

}

xml_map_tree::xpath_error::xpath_error(const string& msg) : general_error(msg) {}

xml_map_tree::cell_position::cell_position() :
    row(-1), col(-1) {}

xml_map_tree::cell_position::cell_position(const pstring& _sheet, spreadsheet::row_t _row, spreadsheet::col_t _col) :
    sheet(_sheet), row(_row), col(_col) {}

xml_map_tree::cell_position::cell_position(const cell_position& r) :
    sheet(r.sheet), row(r.row), col(r.col) {}

xml_map_tree::element_position::element_position() :
    open_begin(nullptr), open_end(nullptr), close_begin(nullptr), close_end(nullptr) {}

xml_map_tree::cell_reference::cell_reference() {}

xml_map_tree::range_reference::range_reference(const cell_position& _pos) :
    pos(_pos), row_size(0) {}

xml_map_tree::linkable::linkable(xmlns_id_t _ns, const pstring& _name, linkable_node_type _node_type) :
    ns(_ns), name(_name), node_type(_node_type) {}

xml_map_tree::attribute::attribute(xmlns_id_t _ns, const pstring& _name, reference_type _ref_type) :
    linkable(_ns, _name, node_attribute), ref_type(_ref_type)
{
    switch (ref_type)
    {
        case reference_cell:
            cell_ref = new cell_reference;
        break;
        case reference_range_field:
            field_ref = new field_in_range;
        break;
        default:
            throw general_error("unexpected reference type in the constructor of attribute.");
    }
}

xml_map_tree::attribute::~attribute()
{
    switch (ref_type)
    {
        case reference_cell:
            delete cell_ref;
        break;
        case reference_range_field:
            delete field_ref;
        break;
        default:
            assert(!"unexpected reference type in the destructor of attribute.");
    }
}

xml_map_tree::element::element(
    xmlns_id_t _ns, const pstring& _name, element_type _elem_type, reference_type _ref_type) :
    linkable(_ns, _name, node_element),
    elem_type(_elem_type),
    ref_type(_ref_type),
    range_parent(nullptr)
{
    if (elem_type == element_unlinked)
    {
        child_elements = new element_store_type;
        return;
    }

    assert(elem_type == element_linked);

    switch (ref_type)
    {
        case reference_cell:
            cell_ref = new cell_reference;
        break;
        case reference_range_field:
            field_ref = new field_in_range;
        break;
        default:
            throw general_error("unexpected reference type in the constructor of element.");
    }
}

xml_map_tree::element::~element()
{
    if (elem_type == element_unlinked)
    {
        delete child_elements;
        return;
    }

    assert(elem_type == element_linked);

    switch (ref_type)
    {
        case reference_cell:
            delete cell_ref;
        break;
        case reference_range_field:
            delete field_ref;
        break;
        default:
            assert(!"unexpected reference type in the destructor of element.");
    }
}

const xml_map_tree::element* xml_map_tree::element::get_child(xmlns_id_t _ns, const pstring& _name) const
{
    if (elem_type != element_unlinked)
        return nullptr;

    assert(child_elements);

    auto it = std::find_if(
        child_elements->begin(), child_elements->end(), find_by_name<element>(_ns, _name));

    return it == child_elements->end() ? nullptr : it->get();
}

bool xml_map_tree::element::unlinked_attribute_anchor() const
{
    return elem_type == element_unlinked && ref_type == reference_unknown && !attributes.empty();
}

xml_map_tree::walker::walker(const xml_map_tree& parent) :
    m_parent(parent) {}
xml_map_tree::walker::walker(const xml_map_tree::walker& r) :
    m_parent(r.m_parent), m_stack(r.m_stack), m_unlinked_stack(r.m_unlinked_stack) {}

void xml_map_tree::walker::reset()
{
    m_stack.clear();
    m_unlinked_stack.clear();
}

const xml_map_tree::element* xml_map_tree::walker::push_element(xmlns_id_t ns, const pstring& name)
{
    if (!m_unlinked_stack.empty())
    {
        // We're still in the unlinked region.
        m_unlinked_stack.push_back(xml_name_t(ns, name));
        return nullptr;
    }

    if (m_stack.empty())
    {
        if (!m_parent.mp_root)
        {
            // Tree is empty.
            m_unlinked_stack.push_back(xml_name_t(ns, name));
            return nullptr;
        }

        const element* p = m_parent.mp_root;
        if (p->ns != ns || p->name != name)
        {
            // Names differ.
            m_unlinked_stack.push_back(xml_name_t(ns, name));
            return nullptr;
        }

        m_stack.push_back(p);
        return p;
    }

    if (m_stack.back()->elem_type == element_unlinked)
    {
        // Check if the current element has a child of the same name.
        const element* p = m_stack.back()->get_child(ns, name);
        if (p)
        {
            m_stack.push_back(p);
            return p;
        }
    }

    m_unlinked_stack.push_back(xml_name_t(ns, name));
    return nullptr;
}

const xml_map_tree::element* xml_map_tree::walker::pop_element(xmlns_id_t ns, const pstring& name)
{
    if (!m_unlinked_stack.empty())
    {
        // We're in the unlinked region.  Pop element from the unlinked stack.
        if (m_unlinked_stack.back().ns != ns || m_unlinked_stack.back().name != name)
            throw general_error("Closing element has a different name than the opening element. (unlinked stack)");

        m_unlinked_stack.pop_back();

        if (!m_unlinked_stack.empty())
            // We are still in the unlinked region.
            return nullptr;

        return m_stack.empty() ? nullptr : m_stack.back();
    }

    if (m_stack.empty())
        throw general_error("Element was popped while the stack was empty.");

    if (m_stack.back()->ns != ns || m_stack.back()->name != name)
        throw general_error("Closing element has a different name than the opening element. (linked stack)");

    m_stack.pop_back();
    return m_stack.empty() ? nullptr : m_stack.back();
}

xml_map_tree::xml_map_tree(xmlns_repository& xmlns_repo) :
    m_xmlns_cxt(xmlns_repo.create_context()), mp_cur_range_ref(nullptr), mp_root(nullptr) {}

xml_map_tree::~xml_map_tree()
{
    std::for_each(m_field_refs.begin(), m_field_refs.end(), map_object_deleter<range_ref_map_type>());
    delete mp_root;
}

void xml_map_tree::set_namespace_alias(const pstring& alias, const pstring& uri)
{
#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "xml_map_tree::set_namespace_alias: alias='" << alias << "', uri='" << uri << "'" << endl;
#endif
    // We need to turn the alias string persistent because the xmlns context
    // doesn't intern the alias strings.
    pstring alias_safe = m_names.intern(alias).first;
    m_xmlns_cxt.push(alias_safe, uri);
}

xmlns_id_t xml_map_tree::get_namespace(const pstring& alias) const
{
    return m_xmlns_cxt.get(alias);
}

void xml_map_tree::set_cell_link(const pstring& xpath, const cell_position& ref)
{
    if (xpath.empty())
        return;

#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "xml_map_tree::set_cell_link: xpath='" << xpath << "' (ref=" << ref << ")" << endl;
#endif

    element_list_type elem_stack;
    linkable* node = get_element_stack(xpath, reference_cell, elem_stack);
    assert(node);
    assert(!elem_stack.empty());
    cell_reference* cell_ref = nullptr;
    switch (node->node_type)
    {
        case node_element:
            assert(static_cast<element*>(node)->cell_ref);
            cell_ref = static_cast<element*>(node)->cell_ref;
        break;
        case node_attribute:
            assert(static_cast<attribute*>(node)->cell_ref);
            cell_ref = static_cast<attribute*>(node)->cell_ref;
        break;
        default:
            throw general_error("unknown node type returned from get_element_stack call in xml_map_tree::set_cell_link().");
    }

    cell_ref->pos = ref;
}

void xml_map_tree::start_range()
{
    m_cur_range_parent.clear();
    mp_cur_range_ref = nullptr;
}

void xml_map_tree::append_range_field_link(const pstring& xpath, const cell_position& pos)
{
    if (xpath.empty())
        return;

    range_reference* range_ref = nullptr;
    range_ref_map_type::iterator it = m_field_refs.lower_bound(pos);
    if (it == m_field_refs.end() || m_field_refs.key_comp()(pos, it->first))
    {
        // This reference does not exist yet.  Insert a new one.

        // Make sure the sheet name string is persistent.
        cell_position pos_safe = pos;
        pos_safe.sheet = m_names.intern(pos.sheet.get(), pos.sheet.size()).first;

        it = m_field_refs.insert(it, range_ref_map_type::value_type(pos_safe, new range_reference(pos_safe)));
    }

    range_ref = it->second;
    assert(range_ref);

    if (!mp_cur_range_ref)
        mp_cur_range_ref = range_ref;

#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "xml_map_tree::append_range_field_link: " << xpath << " (ref=" << pos << ")" << endl;
#endif
    element_list_type elem_stack;
    linkable* node = get_element_stack(xpath, reference_range_field, elem_stack);
    if (elem_stack.size() < 2)
        throw xpath_error("Path of a range field link must be at least 2 levels.");

    switch (node->node_type)
    {
        case node_element:
        {
            element* p = static_cast<element*>(node);
            assert(p && p->ref_type == reference_range_field && p->field_ref);
            p->field_ref->ref = range_ref;
            p->field_ref->column_pos = range_ref->field_nodes.size();

            range_ref->field_nodes.push_back(p);
        }
        break;
        case node_attribute:
        {
            attribute* p = static_cast<attribute*>(node);
            assert(p && p->ref_type == reference_range_field && p->field_ref);
            p->field_ref->ref = range_ref;
            p->field_ref->column_pos = range_ref->field_nodes.size();

            range_ref->field_nodes.push_back(p);
        }
        break;
        default:
            ;
    }

    // Determine the deepest common element for all field link elements in the
    // current range reference.
    if (m_cur_range_parent.empty())
    {
        // First field link in this range.
        element_list_type::iterator it_end = elem_stack.end();
        if (node->node_type == node_element)
            --it_end; // Skip the linked element, which is used as a field in a range.

        --it_end; // Skip the next-up element, which is used to group a single record entry.
        m_cur_range_parent.assign(elem_stack.begin(), it_end);
#if ORCUS_DEBUG_XML_MAP_TREE
        print_element_stack(cout, m_cur_range_parent);
        cout << endl;
#endif
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

        if (m_cur_range_parent.empty())
            throw xpath_error("Two field links in the same range reference must at least share the first level of their paths.");
    }
}

void xml_map_tree::commit_range()
{
    if (!mp_cur_range_ref)
        // Nothing to commit.
        return;

#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "parent element path for this range: ";
    element_list_type::iterator it = m_cur_range_parent.begin(), it_end = m_cur_range_parent.end();
    for (; it != it_end; ++it)
        cout << "/" << (**it).name;
    cout << endl;
#endif

    assert(!m_cur_range_parent.empty());
    // Mark the range parent element.
    m_cur_range_parent.back()->range_parent = mp_cur_range_ref;
}

const xml_map_tree::linkable* xml_map_tree::get_link(const pstring& xpath) const
{
    if (!mp_root)
        return nullptr;

    if (xpath.empty())
        return nullptr;

#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "xml_map_tree::get_link: xpath = '" << xpath << "'" << endl;
#endif
    const linkable* cur_node = mp_root;

    xpath_parser parser(m_xmlns_cxt, xpath.get(), xpath.size());

    // Check the root element first.
    xpath_parser::token token = parser.next();
    if (cur_node->ns != token.ns || cur_node->name != token.name)
        // Root element name doesn't match.
        return nullptr;

#if ORCUS_DEBUG_XML_MAP_TREE
    cout << "xml_map_tree::get_link: root = (ns=" << token.ns << ", name=" << token.name << ")" << endl;
#endif
    for (token = parser.next(); !token.name.empty(); token = parser.next())
    {
        if (token.attribute)
        {
            // The current node should be an element and should have an attribute of the same name.
            if (cur_node->node_type != node_element)
                return nullptr;

            const element* elem = static_cast<const element*>(cur_node);
            const attribute_store_type& attrs = elem->attributes;
            auto it = std::find_if(
                attrs.begin(), attrs.end(), find_by_name<attribute>(token.ns, token.name));

            if (it == attrs.end())
                // No such attribute exists.
                return nullptr;

            return it->get();
        }

        // See if an element of this name exists below the current element.

        if (cur_node->node_type != node_element)
            return nullptr;

        const element* elem = static_cast<const element*>(cur_node);
        if (elem->elem_type != element_unlinked)
            return nullptr;

        if (!elem->child_elements)
            return nullptr;

        auto it = std::find_if(
            elem->child_elements->begin(), elem->child_elements->end(),
            find_by_name<element>(token.ns, token.name));

        if (it == elem->child_elements->end())
            // No such child element exists.
            return nullptr;

        cur_node = it->get();
    }

    if (cur_node->node_type != node_element || static_cast<const element*>(cur_node)->elem_type == element_unlinked)
        // Non-leaf elements are not links.
        return nullptr;

    return cur_node;
}

xml_map_tree::walker xml_map_tree::get_tree_walker() const
{
    return walker(*this);
}

xml_map_tree::range_ref_map_type& xml_map_tree::get_range_references()
{
    return m_field_refs;
}

pstring xml_map_tree::intern_string(const pstring& str) const
{
    return m_names.intern(str).first;
}

xml_map_tree::linkable* xml_map_tree::get_element_stack(
    const pstring& xpath, reference_type ref_type, element_list_type& elem_stack)
{
    assert(!xpath.empty());
    xpath_parser parser(m_xmlns_cxt,xpath.get(), xpath.size());

    element_list_type elem_stack_new;

    // Get the root element first.
    xpath_parser::token token = parser.next();
    if (mp_root)
    {
        // Make sure the root element's names are the same.
        if (mp_root->ns != token.ns || mp_root->name != token.name)
            throw xpath_error("path begins with inconsistent root level name.");
    }
    else
    {
        // First time the root element is encountered.
        if (token.attribute)
            throw xpath_error("root element cannot be an attribute.");

        mp_root = new element(
            token.ns, m_names.intern(token.name.get(), token.name.size()).first,
            element_unlinked, reference_unknown);
    }

    elem_stack_new.push_back(mp_root);
    element* cur_element = elem_stack_new.back();
    assert(cur_element);
    assert(cur_element->child_elements);

    token = parser.next();
    for (xpath_parser::token token_next = parser.next(); !token_next.name.empty(); token_next = parser.next())
    {
        // Check if the current element contains a child element of the same name.
        if (token.attribute)
            throw xpath_error("attribute must always be at the end of the path.");

        element_store_type& children = *cur_element->child_elements;
        auto it = std::find_if(
            children.begin(), children.end(), find_by_name<element>(token.ns, token.name));
        if (it == children.end())
        {
            // Insert a new element of this name.
            children.push_back(
                orcus::make_unique<element>(
                    token.ns, m_names.intern(token.name.get(), token.name.size()).first,
                    element_unlinked, reference_unknown));
            cur_element = children.back().get();
        }
        else
            cur_element = it->get();

        elem_stack_new.push_back(cur_element);
        token = token_next;
    }

    assert(cur_element);

    // Insert a leaf node.

    linkable* ret = nullptr;
    if (token.attribute)
    {
        // This is an attribute.  Insert it into the current element.
        attribute_store_type& attrs = cur_element->attributes;

        // Check if an attribute of the same name already exists.
        auto it = std::find_if(
            attrs.begin(), attrs.end(), find_by_name<attribute>(token.ns, token.name));
        if (it != attrs.end())
            throw xpath_error("This attribute is already linked.  You can't link the same attribute twice.");

        attrs.push_back(
            orcus::make_unique<attribute>(
                token.ns, m_names.intern(token.name.get(), token.name.size()).first, ref_type));

        ret = attrs.back().get();
    }
    else
    {
        // Check if an element of the same name already exists.
        element_store_type& children = *cur_element->child_elements;
        auto it = std::find_if(
            children.begin(), children.end(), find_by_name<element>(token.ns, token.name));
        if (it == children.end())
        {
            // No element of that name exists.
            children.push_back(
                orcus::make_unique<element>(
                    token.ns, m_names.intern(token.name.get(), token.name.size()).first,
                    element_linked, ref_type));

            elem_stack_new.push_back(children.back().get());
            ret = children.back().get();
        }
        else
        {
            // This element already exists.  Check if this is already linked.
            element& elem = **it;
            if (elem.ref_type != reference_unknown || elem.elem_type != element_unlinked)
                throw xpath_error("This element is already linked.  You can't link the same element twice.");

            // Turn this existing non-linked element into a linked one.
            delete elem.child_elements;
            elem.elem_type = element_linked;
            elem.ref_type = ref_type;
            switch (ref_type)
            {
                case reference_cell:
                    elem.cell_ref = new cell_reference;
                break;
                case reference_range_field:
                    elem.field_ref = new field_in_range;
                break;
                default:
                    throw general_error("Unknown reference type in xml_map_tree::get_element_stack.");
            }

            elem_stack_new.push_back(&elem);
            ret = &elem;
        }
    }

    elem_stack.swap(elem_stack_new);

    return ret;
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_position& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::linkable& link)
{
    if (!link.ns_alias.empty())
        os << link.ns_alias << ':';
    os << link.name;
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

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
