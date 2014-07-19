/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_table_context.hpp"
#include "xlsx_autofilter_context.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_token_constants.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xlsx_table_context::xlsx_table_context(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_table* table) :
    xml_context_base(session_cxt, tokens), mp_table(table) {}

xlsx_table_context::~xlsx_table_context() {}

bool xlsx_table_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_ooxml_xlsx && name == XML_autoFilter)
        return false;

    return true;
}

xml_context_base* xlsx_table_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_ooxml_xlsx && name == XML_autoFilter)
    {
        mp_child.reset(new xlsx_autofilter_context(get_session_context(), get_tokens()));
        mp_child->transfer_common(*this);
        return mp_child.get();
    }
    return NULL;
}

void xlsx_table_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
    if (ns == NS_ooxml_xlsx && name == XML_autoFilter)
    {
        const xlsx_autofilter_context& cxt = static_cast<const xlsx_autofilter_context&>(*child);
        const pstring& ref_range = cxt.get_ref_range();
        const xlsx_autofilter_context::column_filters_type& filters = cxt.get_column_filters();

        // TODO : Push autofilter data to the model.

        cout << "* autofilter (range=" << ref_range << ")" << endl;

        xlsx_autofilter_context::column_filters_type::const_iterator it = filters.begin(), it_end = filters.end();
        for (; it != it_end; ++it)
        {
            spreadsheet::col_t col = it->first;
            const xlsx_autofilter_context::match_values_type& mv = it->second;

            cout << "  * column id: " << col << endl;
            xlsx_autofilter_context::match_values_type::const_iterator itmv = mv.begin(), itmv_end = mv.end();
            for (; itmv != itmv_end; ++itmv)
            {
                const pstring& v = *itmv;
                cout << "    * match: " << v << endl;
            }
        }
    }
}

void xlsx_table_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns != NS_ooxml_xlsx)
        return;

    switch (name)
    {
        case XML_table:
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
        break;
        default:
            warn_unhandled();
    }

}

bool xlsx_table_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_table_context::characters(const pstring& str, bool transient)
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
