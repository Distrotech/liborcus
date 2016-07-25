/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_context_base.hpp"
#include "orcus/exception.hpp"
#include "orcus/tokens.hpp"

#include <iostream>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

void print_stack(const tokens& tokens, const xml_elem_stack_t& elem_stack, const xmlns_context* ns_cxt)
{
    cerr << "[ ";
    xml_elem_stack_t::const_iterator itr, itr_beg = elem_stack.begin(), itr_end = elem_stack.end();
    for (itr = itr_beg; itr != itr_end; ++itr)
    {
        if (itr != itr_beg)
            cerr << " -> ";

        xmlns_id_t ns = itr->first;
        if (ns_cxt)
        {
            pstring alias = ns_cxt->get_alias(ns);
            if (!alias.empty())
                cerr << alias << ":";
        }
        else
            cerr << ns << ":";

        cerr << tokens.get_token_name(itr->second);
    }
    cerr << " ]";
}

}

xml_context_base::xml_context_base(session_context& session_cxt, const tokens& tokens) :
    mp_ns_cxt(nullptr), m_session_cxt(session_cxt), m_tokens(tokens) {}

xml_context_base::~xml_context_base()
{
}

void xml_context_base::set_ns_context(const xmlns_context* p)
{
    mp_ns_cxt = p;
}

void xml_context_base::set_config(const config& opt)
{
    m_config = opt;
}

void xml_context_base::transfer_common(const xml_context_base& parent)
{
    m_config = parent.m_config;
    mp_ns_cxt = parent.mp_ns_cxt;
}

session_context& xml_context_base::get_session_context()
{
    return m_session_cxt;
}

const tokens& xml_context_base::get_tokens() const
{
    return m_tokens;
}

xml_token_pair_t xml_context_base::push_stack(xmlns_id_t ns, xml_token_t name)
{
    xml_token_pair_t parent = m_stack.empty() ? xml_token_pair_t(XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN) : m_stack.back();
    m_stack.push_back(xml_token_pair_t(ns, name));
    return parent;
}

bool xml_context_base::pop_stack(xmlns_id_t ns, xml_token_t name)
{
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

xml_token_pair_t& xml_context_base::get_parent_element()
{
    if(m_stack.size() < 2)
        throw general_error("element stack has no parent element");

    return m_stack[m_stack.size() - 2];
}

const xml_token_pair_t& xml_context_base::get_parent_element() const
{
    if(m_stack.size() < 2)
        throw general_error("element stack has no parent element");

    return m_stack[m_stack.size() - 2];
}

void xml_context_base::warn_unhandled() const
{
    if (!m_config.debug)
        return;

    cerr << "warning: unhandled element ";
    print_stack(m_tokens, m_stack, mp_ns_cxt);
    cerr << endl;
}

void xml_context_base::warn_unexpected() const
{
    if (!m_config.debug)
        return;

    cerr << "warning: unexpected element ";
    print_stack(m_tokens, m_stack, mp_ns_cxt);
    cerr << endl;
}

void xml_context_base::warn(const char* msg) const
{
    if (!m_config.debug)
        return;

    cerr << "warning: " << msg << endl;
}

void xml_context_base::xml_element_expected(
    const xml_token_pair_t& elem, xmlns_id_t ns, xml_token_t name,
    const string* error)
{
    if (!m_config.structure_check)
        return;

    if (elem.first == ns && elem.second == name)
        // This is an expected element.  Good.
        return;

    if (error)
    {
        throw xml_structure_error(*error);
    }

    // Create a generic error message.
    ostringstream os;
    os << "element '" << ns << ":" << m_tokens.get_token_name(name) << "' expected, but '";
    os << elem.first << ":" << m_tokens.get_token_name(elem.second) << "' encountered.";
    throw xml_structure_error(os.str());
}

void xml_context_base::xml_element_expected(
    const xml_token_pair_t& elem, const xml_elem_stack_t& expected_elems)
{
    if (!m_config.structure_check)
        return;

    xml_elem_stack_t::const_iterator itr = expected_elems.begin(), itr_end = expected_elems.end();
    for (; itr != itr_end; ++itr)
    {
        if (elem == *itr)
            return;
    }

    // Create a generic error message.
    ostringstream os;
    os << "unexpected element encountered: " << elem.first << ":" << m_tokens.get_token_name(elem.second);
    throw xml_structure_error(os.str());
}

const config& xml_context_base::get_config() const
{
    return m_config;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
