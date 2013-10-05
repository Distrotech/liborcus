/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_para_context.hpp"
#include "odf_token_constants.hpp"
#include "odf_namespace_types.hpp"
#include "xml_context_global.hpp"

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/exception.hpp"

#include <iostream>
#include <cassert>

using namespace std;

namespace orcus {

text_para_context::text_para_context(
    session_context& session_cxt, const tokens& tokens,
    spreadsheet::iface::import_shared_strings* ssb, odf_styles_map_type& styles) :
    xml_context_base(session_cxt, tokens),
    mp_sstrings(ssb), m_styles(styles),
    m_string_index(0), m_has_content(false)
{
}

text_para_context::~text_para_context()
{
}

bool text_para_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* text_para_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void text_para_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
    // not implemented yet.
}

void text_para_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_odf_text)
    {
        switch (name)
        {
            case XML_p:
                // paragraph
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            break;
            case XML_span:
            {
                // text span.
                xml_element_expected(parent, NS_odf_text, XML_p);
                flush_segment();
                pstring style_name =
                    for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_odf_text, XML_style_name)).get_value();
                m_span_stack.push_back(style_name);

            }
            break;
            case XML_s:
                // control character.  ignored for now.
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool text_para_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_text)
    {
        switch (name)
        {
            case XML_p:
            {
                // paragraph
                flush_segment();
                m_string_index = mp_sstrings->commit_segments();
            }
            break;
            case XML_span:
            {
                // text span.
                if (m_span_stack.empty())
                    throw xml_structure_error("</text:span> encountered without matching opening element.");

                flush_segment();
                m_span_stack.pop_back();
            }
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void text_para_context::characters(const pstring& str, bool transient)
{
    if (transient)
        m_contents.push_back(m_pool.intern(str).first);
    else
        m_contents.push_back(str);
}

void text_para_context::reset()
{
    m_string_index = 0;
    m_has_content = false;
    m_pool.clear();
    m_contents.clear();
}

size_t text_para_context::get_string_index() const
{
    return m_string_index;
}

bool text_para_context::empty() const
{
    return !m_has_content;
}

void text_para_context::flush_segment()
{
    if (m_contents.empty())
        // No content to flush.
        return;

    m_has_content = true;

    const odf_style* style = NULL;
    if (!m_span_stack.empty())
    {
        pstring style_name = m_span_stack.back();
        odf_styles_map_type::const_iterator it = m_styles.find(style_name);
        if (it != m_styles.end())
            style = it->second;
    }

    if (style && style->family == style_family_text)
    {
        const odf_style::text* data = style->text_data;
        mp_sstrings->set_segment_font(data->font);
    }

    vector<pstring>::const_iterator it = m_contents.begin(), it_end = m_contents.end();
    for (; it != it_end; ++it)
    {
        const pstring& ps = *it;
        mp_sstrings->append_segment(ps.get(), ps.size());
    }

    m_contents.clear();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
