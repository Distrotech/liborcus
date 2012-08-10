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

namespace orcus {

dom_tree::attr::attr(const pstring& _ns, const pstring& _name, const pstring& _value) :
    ns(_ns), name(_name), value(_value) {}

dom_tree::node::~node() {}

dom_tree::element::element(const pstring& _ns, const pstring& _name) : ns(_ns), name(_name) {}
dom_tree::element::~element() {}

dom_tree::content::content(const pstring& _value) : value(_value) {}
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

}
