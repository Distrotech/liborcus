/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_stream_handler.hpp"
#include "xml_context_base.hpp"

#include "orcus/exception.hpp"

namespace orcus {

xml_stream_handler::xml_stream_handler(xml_context_base* root_context) :
    mp_ns_cxt(NULL),
    mp_root_context(root_context)
{
    m_context_stack.push_back(root_context);
}

xml_stream_handler::~xml_stream_handler()
{
    delete mp_root_context;
}

void xml_stream_handler::start_document()
{
}

void xml_stream_handler::end_document()
{
}

void xml_stream_handler::start_element(const sax_token_parser_element& elem)
{
    xml_context_base& cur = get_current_context();
    if (!cur.can_handle_element(elem.ns, elem.name))
    {
        m_context_stack.push_back(cur.create_child_context(elem.ns, elem.name));
        m_context_stack.back()->set_ns_context(mp_ns_cxt);
    }

    get_current_context().start_element(elem.ns, elem.name, elem.attrs);
}

void xml_stream_handler::end_element(const sax_token_parser_element& elem)
{
    bool ended = get_current_context().end_element(elem.ns, elem.name);

    if (ended)
    {
        size_t n = m_context_stack.size();

        if (n > 1)
        {
            // Call end_child_context of the parent context to provide a way for
            // the two adjacent contexts to communicate with each other.
            context_stack_type::reverse_iterator itr_cur = m_context_stack.rbegin();
            context_stack_type::reverse_iterator itr_par = itr_cur + 1;
            (*itr_par)->end_child_context(elem.ns, elem.name, *itr_cur);
        }

        m_context_stack.pop_back();
    }
}

void xml_stream_handler::characters(const pstring& str, bool transient)
{
    get_current_context().characters(str, transient);
}

void xml_stream_handler::set_ns_context(const xmlns_context* p)
{
    mp_ns_cxt = p;
    if (!m_context_stack.empty())
        m_context_stack.back()->set_ns_context(p);
}

void xml_stream_handler::set_config(const config& opt)
{
    m_config = opt;
    if (!m_context_stack.empty())
        m_context_stack.back()->set_config(m_config);
}

xml_context_base& xml_stream_handler::get_current_context()
{
    if (m_context_stack.empty())
        return *mp_root_context;

    return *m_context_stack.back();
}

xml_context_base& xml_stream_handler::get_root_context()
{
    return *mp_root_context;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
