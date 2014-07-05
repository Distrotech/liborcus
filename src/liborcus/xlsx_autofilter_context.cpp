/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_autofilter_context.hpp"
#include "xml_context_global.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_token_constants.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xlsx_autofilter_context::xlsx_autofilter_context(session_context& session_cxt, const tokens& tokens) :
    xml_context_base(session_cxt, tokens), m_cur_col(-1) {}

xlsx_autofilter_context::~xlsx_autofilter_context() {}

bool xlsx_autofilter_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_autofilter_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_autofilter_context::end_child_context(
    xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_autofilter_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    if (ns != NS_ooxml_xlsx)
        return;

    switch (name)
    {
        case XML_autoFilter:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            m_ref_range = for_each(
                attrs.begin(), attrs.end(),
                single_attr_getter(m_pool, NS_ooxml_xlsx, XML_ref)).get_value();
        }
        break;
        case XML_filterColumn:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_autoFilter);
            m_cur_col = for_each(
                attrs.begin(), attrs.end(),
                single_long_attr_getter(NS_ooxml_xlsx, XML_colId)).get_value();
        }
        break;
        case XML_filters:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_filterColumn);
        break;
        case XML_filter:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_filters);
            pstring val = for_each(
                attrs.begin(), attrs.end(),
                single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            if (!val.empty())
                m_cur_match_values.push_back(val);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_autofilter_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_filterColumn:
            {
                if (m_cur_col >= 0)
                {
                    m_column_filters.insert(
                        column_filters_type::value_type(m_cur_col, m_cur_match_values));
                }
                m_cur_col = -1;
                m_cur_match_values.clear();
            }
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void xlsx_autofilter_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

const xlsx_autofilter_context::column_filters_type& xlsx_autofilter_context::get_column_filters() const
{
    return m_column_filters;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
