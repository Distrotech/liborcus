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

#include "orcus/dom_tree.hpp"
#include "orcus/exception.hpp"
#include "orcus/xml_namespace.hpp"

#include "orcus/string_pool.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

/**
 * Escape certain characters with backslash (\).
 */
void escape(ostream& os, const pstring& val)
{
    if (val.empty())
        return;

    const char* p = &val[0];
    const char* p_end = p + val.size();
    for (; p != p_end; ++p)
    {
        if (*p == '"')
            os << "\\\"";
        else if (*p == '\\')
            os << "\\\\";
        else
            os << *p;
    }
}

}

struct dom_tree_impl
{
    xmlns_context& m_ns_cxt;
    string_pool m_pool;

    dom_tree::attrs_type m_doc_attrs;
    dom_tree::attrs_type m_cur_attrs;
    dom_tree::element_stack_type m_elem_stack;
    dom_tree::element* m_root;

    dom_tree_impl(xmlns_context& cxt) : m_ns_cxt(cxt), m_root(NULL) {}

    ~dom_tree_impl()
    {
        delete m_root;
    }
};

dom_tree::entity_name::entity_name() : ns(XMLNS_UNKNOWN_ID) {}

dom_tree::entity_name::entity_name(xmlns_id_t _ns, const pstring& _name) :
    ns(_ns), name(_name) {}

void dom_tree::entity_name::print(std::ostream& os, const xmlns_context& cxt) const
{
    if (ns)
    {
        size_t index = cxt.get_index(ns);
        if (index != xmlns_context::index_not_found)
            os << "ns" << index << ':';
    }
    os << name;
}

dom_tree::attr::attr(xmlns_id_t _ns, const pstring& _name, const pstring& _value) :
    name(_ns, _name), value(_value) {}

void dom_tree::attr::print(std::ostream& os, const xmlns_context& cxt) const
{
    name.print(os, cxt);
    os << "=\"";
    escape(os, value);
    os << '"';
}

dom_tree::node::~node() {}

dom_tree::element::element(xmlns_id_t _ns, const pstring& _name) : node(node_element), name(_ns, _name) {}

void dom_tree::element::print(ostream& os, const xmlns_context& cxt) const
{
    name.print(os, cxt);
}

dom_tree::element::~element() {}

dom_tree::content::content(const pstring& _value) : node(node_content), value(_value) {}

void dom_tree::content::print(ostream& os, const xmlns_context& /*cxt*/) const
{
    os << '"';
    escape(os, value);
    os << '"';
}

dom_tree::content::~content() {}

dom_tree::dom_tree(xmlns_context& cxt) : mp_impl(new dom_tree_impl(cxt)) {}

dom_tree::~dom_tree() { delete mp_impl; }

void dom_tree::end_declaration()
{
    mp_impl->m_doc_attrs.swap(mp_impl->m_cur_attrs);
}

void dom_tree::start_element(xmlns_id_t ns, const pstring& name)
{
    // These strings must be persistent.
    pstring name_safe = mp_impl->m_pool.intern(name).first;

    element* p = NULL;
    if (!mp_impl->m_root)
    {
        // This must be the root element!
        mp_impl->m_root = new element(ns, name_safe);
        mp_impl->m_elem_stack.push_back(mp_impl->m_root);
        p = mp_impl->m_elem_stack.back();
        p->attrs.swap(mp_impl->m_cur_attrs);
        return;
    }

    // Append new element as a child element of the current element.
    p = mp_impl->m_elem_stack.back();
    p->child_nodes.push_back(new element(ns, name_safe));
    p = static_cast<element*>(&p->child_nodes.back());
    p->attrs.swap(mp_impl->m_cur_attrs);
    mp_impl->m_elem_stack.push_back(p);
}

void dom_tree::end_element(xmlns_id_t ns, const pstring& name)
{
    const element* p = mp_impl->m_elem_stack.back();
    if (p->name.ns != ns || p->name.name != name)
        throw general_error("non-matching end element.");

    mp_impl->m_elem_stack.pop_back();
}

void dom_tree::set_characters(const pstring& val)
{
    if (mp_impl->m_elem_stack.empty())
        // No root element has been encountered.  Ignore this.
        return;

    pstring val2 = val.trim();
    if (val2.empty())
        return;

    element* p = mp_impl->m_elem_stack.back();
    val2 = mp_impl->m_pool.intern(val2).first; // Make sure the string is persistent.
    p->child_nodes.push_back(new content(val2));
}

void dom_tree::set_attribute(xmlns_id_t ns, const pstring& name, const pstring& val)
{
    // These strings must be persistent.
    pstring name2 = mp_impl->m_pool.intern(name).first;
    pstring val2 = mp_impl->m_pool.intern(val).first;

    mp_impl->m_cur_attrs.push_back(attr(ns, name2, val2));
}

namespace {

struct scope : boost::noncopyable
{
    typedef std::vector<const dom_tree::node*> nodes_type;
    string name;
    nodes_type nodes;
    nodes_type::const_iterator current_pos;

    scope(const string& _name, dom_tree::node* _node) :
        name(_name)
    {
        nodes.push_back(_node);
        current_pos = nodes.begin();
    }

    scope(const string& _name) : name(_name) {}
};

typedef boost::ptr_vector<scope> scopes_type;

void print_scope(ostream& os, const scopes_type& scopes)
{
    if (scopes.empty())
        throw general_error("scope stack shouldn't be empty while dumping tree.");

    // Skip the first scope which is root.
    scopes_type::const_iterator it = scopes.begin(), it_end = scopes.end();
    for (++it; it != it_end; ++it)
        os << "/" << it->name;
}

struct sort_by_name : std::binary_function<dom_tree::attr, dom_tree::attr, bool>
{
    bool operator() (const dom_tree::attr& left, const dom_tree::attr& right) const
    {
        return left.name.name < right.name.name;
    }
};

}

void dom_tree::dump_compact(ostream& os) const
{
    if (!mp_impl->m_root)
        return;

    // Dump namespaces first.
    mp_impl->m_ns_cxt.dump(os);

    scopes_type scopes;

    scopes.push_back(new scope(string(), mp_impl->m_root));
    while (!scopes.empty())
    {
        bool new_scope = false;

        // Iterate through all elements in the current scope.
        scope& cur_scope = scopes.back();
        for (; cur_scope.current_pos != cur_scope.nodes.end(); ++cur_scope.current_pos)
        {
            const node* this_node = *cur_scope.current_pos;
            assert(this_node);
            print_scope(os, scopes);
            if (this_node->type == node_content)
            {
                // This is a text content.
                this_node->print(os, mp_impl->m_ns_cxt);
                os << endl;
                continue;
            }

            assert(this_node->type == node_element);
            const element* elem = static_cast<const element*>(this_node);
            os << "/";
            elem->print(os, mp_impl->m_ns_cxt);
            os << endl;

            {
                // Dump attributes.
                attrs_type attrs = elem->attrs;
                sort(attrs.begin(), attrs.end(), sort_by_name());
                attrs_type::const_iterator it = attrs.begin(), it_end = attrs.end();
                for (; it != it_end; ++it)
                {
                    print_scope(os, scopes);
                    os << "/";
                    elem->print(os, mp_impl->m_ns_cxt);
                    os << "@";
                    it->print(os, mp_impl->m_ns_cxt);
                    os << endl;
                }
            }

            if (elem->child_nodes.empty())
                continue;

            // This element has child nodes.  Push a new scope and populate it
            // with all child elements, but skip content nodes.
            dom_tree::nodes_type::const_iterator it = elem->child_nodes.begin(), it_end = elem->child_nodes.end();
            scope::nodes_type nodes;
            for (; it != it_end; ++it)
                nodes.push_back(&(*it));

            assert(!nodes.empty());

            // Push a new scope, and restart the loop with the new scope.
            ++cur_scope.current_pos;
            ostringstream elem_name;
            elem->print(elem_name, mp_impl->m_ns_cxt);
            scopes.push_back(new scope(elem_name.str()));
            scope& child_scope = scopes.back();
            child_scope.nodes.swap(nodes);
            child_scope.current_pos = child_scope.nodes.begin();

            new_scope = true;
            break;
        }

        if (new_scope)
            continue;

        scopes.pop_back();
    }
}

}
