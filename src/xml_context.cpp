/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include "orcus/xml_context.hpp"
#include "orcus/global.hpp"
#include "orcus/tokens.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

void print_stack(const tokens& tokens, const xml_elem_stack_t& elem_stack)
{
    cerr << "[ ";
    xml_elem_stack_t::const_iterator itr, itr_beg = elem_stack.begin(), itr_end = elem_stack.end();
    for (itr = itr_beg; itr != itr_end; ++itr)
    {
        if (itr != itr_beg)
            cerr << " -> ";
        cerr << tokens.get_nstoken_name(itr->first) << ":" << tokens.get_token_name(itr->second);
    }
    cerr << " ]";
}

}

xml_context_base::xml_context_base(const tokens& tokens) :
    m_tokens(tokens),
    m_default_ns(XMLNS_UNKNOWN_TOKEN)
    {}

xml_context_base::~xml_context_base()
{
}

const tokens& xml_context_base::get_tokens() const
{
    return m_tokens;
}

xml_token_pair_t xml_context_base::push_stack(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_UNKNOWN_TOKEN)
        ns = m_default_ns;

    xml_token_pair_t parent = m_stack.empty() ? xml_token_pair_t(XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN) : m_stack.back();
    m_stack.push_back(xml_token_pair_t(ns, name));
    return parent;
}

bool xml_context_base::pop_stack(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_UNKNOWN_TOKEN)
        ns = m_default_ns;

    const xml_token_pair_t& r = m_stack.back();

    if (ns != r.first || name != r.second)
        throw general_error("mismatched element name");

    m_stack.pop_back();
    return m_stack.empty();
}

xml_token_pair_t& xml_context_base::get_current_element()
{
    if (m_stack.empty())
        throw general_error("element stack is empty!");
    return m_stack.back();
}

const xml_token_pair_t& xml_context_base::get_current_element() const
{
    if (m_stack.empty())
        throw general_error("element stack is empty!");
    return m_stack.back();
}

void xml_context_base::warn_unhandled() const
{
    cerr << "warning: unhandled element ";
    print_stack(m_tokens, m_stack);
    cerr << endl;
}

void xml_context_base::warn_unexpected() const
{
    cerr << "warning: unexpected element ";
    print_stack(m_tokens, m_stack);
    cerr << endl;
}

void xml_context_base::set_default_ns(xmlns_token_t ns)
{
    m_default_ns = ns;
}

xmlns_token_t xml_context_base::get_default_ns() const
{
    return m_default_ns;
}

void xml_context_base::xml_element_expected(
    const xml_token_pair_t& elem, xmlns_token_t ns, xml_token_t name, 
    const string* error)
{
    if (elem.first == ns && elem.second == name)
        // This is an expected element.  Good.
        return;

    if (error)
    {
        throw xml_structure_error(*error);
    }

    // Create a generic error message.
    ostringstream os;
    os << "element '" << m_tokens.get_nstoken_name(ns) << ":" << m_tokens.get_token_name(name) << "' expected, but '";
    os << m_tokens.get_nstoken_name(elem.first) << ":" << m_tokens.get_token_name(elem.second) << "' encountered.";
    throw xml_structure_error(os.str());
}

}
