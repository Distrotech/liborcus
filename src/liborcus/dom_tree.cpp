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

#include <iostream>
#include <sstream>

using namespace std;

namespace orcus {

dom_tree::attr::attr(const pstring& _ns, const pstring& _name, const pstring& _value) :
    ns(_ns), name(_name), value(_value) {}

dom_tree::node::~node() {}

dom_tree::element::element(const pstring& _ns, const pstring& _name) : node(node_element), ns(_ns), name(_name) {}

string dom_tree::element::print() const
{
    ostringstream os;
    if (!ns.empty())
        os << ns << ':';
    os << name;
    return os.str();
}

dom_tree::element::~element() {}

dom_tree::content::content(const pstring& _value) : node(node_content), value(_value) {}

string dom_tree::content::print() const
{
    ostringstream os;
    os << '"' << value << '"';
    return os.str();
}

dom_tree::content::~content() {}

dom_tree::dom_tree() :
    m_root(NULL)
{
}

dom_tree::~dom_tree()
{
    delete m_root;
}

void dom_tree::end_declaration()
{
    m_doc_attrs.swap(m_cur_attrs);
}

void dom_tree::start_element(const pstring& ns, const pstring& name)
{
    element* p = NULL;
    if (!m_root)
    {
        // This must be the root element!
        m_root = new element(ns, name);
        m_elem_stack.push_back(m_root);
        p = m_elem_stack.back();
        p->attrs.swap(m_cur_attrs);
        return;
    }

    // Append new element as a child element of the current element.
    p = m_elem_stack.back();
    p->child_nodes.push_back(new element(ns, name));
    p = static_cast<element*>(&p->child_nodes.back());
    p->attrs.swap(m_cur_attrs);
    m_elem_stack.push_back(p);
}

void dom_tree::end_element(const pstring& ns, const pstring& name)
{
    const element* p = m_elem_stack.back();
    if (p->ns != ns || p->name != name)
        throw general_error("non-matching end element.");

    m_elem_stack.pop_back();
}

void dom_tree::set_characters(const pstring& val)
{
    if (m_elem_stack.empty())
        // No root element has been encountered.  Ignore this.
        return;

    pstring val2 = val.trim();
    if (val2.empty())
        return;

    element* p = m_elem_stack.back();
    p->child_nodes.push_back(new content(val));
}

void dom_tree::set_attribute(const pstring& ns, const pstring& name, const pstring& val)
{
    m_cur_attrs.push_back(attr(ns, name, val));
}

namespace {

struct scope : boost::noncopyable
{
    typedef std::vector<const dom_tree::element*> elements_type;
    string name;
    elements_type elements;
    elements_type::const_iterator current_pos;

    scope(const string& _name, dom_tree::element* _element) :
        name(_name)
    {
        elements.push_back(_element);
        current_pos = elements.begin();
    }

    scope(const string& _name) : name(_name) {}
};

typedef boost::ptr_vector<scope> scopes_type;

void print_scope(ostringstream& os, const scopes_type& scopes)
{
    if (scopes.empty())
        throw general_error("scope stack shouldn't be empty while dumping tree.");

    // Skip the first scope which is root.
    scopes_type::const_iterator it = scopes.begin(), it_end = scopes.end();
    for (++it; it != it_end; ++it)
        os << "/" << it->name;
}

}

void dom_tree::dump() const
{
    if (!m_root)
        return;

    scopes_type scopes;
    ostringstream os;

    // Print the element.

    // Print the attributes.

    // Move on to child nodes.
    scopes.push_back(new scope(string(), m_root));
    while (!scopes.empty())
    {
        bool new_scope = false;

        // Iterate through all elements in the current scope.
        scope& cur_scope = scopes.back();
        for (; cur_scope.current_pos != cur_scope.elements.end(); ++cur_scope.current_pos)
        {
            const element* elem = *cur_scope.current_pos;
            assert(elem);
            print_scope(os, scopes);
            os << "/" << elem->name << endl;

            if (elem->child_nodes.empty())
                continue;

            // This element has child nodes.  Push a new scope and populate it
            // with all child elements, but skip content nodes.
            dom_tree::nodes_type::const_iterator it = elem->child_nodes.begin(), it_end = elem->child_nodes.end();
            scope::elements_type elements;
            for (; it != it_end; ++it)
            {
                switch (it->type)
                {
                    case node_element:
                    {
                        const element* p = static_cast<const element*>(&(*it));
                        elements.push_back(p);
                    }
                    break;
                    case node_content:
                    {
                        print_scope(os, scopes);
                        const content& con = static_cast<const content&>(*it);
                        os << "/" << elem->name << '"' << con.value << '"' << endl;
                    }
                    break;
                    default:
                        throw general_error("unknown node type during the dump.");
                }
            }

            if (elements.empty())
                continue;

            // Push a new scope, and restart the loop with the new scope.
            ++cur_scope.current_pos;
            scopes.push_back(new scope(elem->name.str()));
            scopes.back().elements.swap(elements);
            scopes.back().current_pos = scopes.back().elements.begin();
            new_scope = true;
            break;
        }

        if (new_scope)
            continue;

        scopes.pop_back();
    }

    cout << os.str();
}

}
