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
#include "session_context.hpp"
#include "xml_context_global.hpp"

#include "orcus/measurement.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

class table_attr_parser : public unary_function<xml_token_attr_t, void>
{
    string_pool* m_pool;

    long m_id;
    long m_totals_row_count;

    pstring m_name;
    pstring m_display_name;
    pstring m_ref;

public:
    table_attr_parser(string_pool* pool) : m_pool(pool), m_id(-1), m_totals_row_count(-1) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_id:
                m_id = to_long(attr.value);
            break;
            case XML_totalsRowCount:
                m_totals_row_count = to_long(attr.value);
            break;
            case XML_name:
                m_name = attr.value;
                if (attr.transient)
                    m_name = m_pool->intern(m_name).first;
            break;
            case XML_displayName:
                m_display_name = attr.value;
                if (attr.transient)
                    m_display_name = m_pool->intern(m_display_name).first;
            break;
            case XML_ref:
                m_ref = attr.value;
                if (attr.transient)
                    m_ref = m_pool->intern(m_ref).first;
            break;
            default:
                ;
        }
    }

    long get_id() const { return m_id; }
    long get_totals_row_count() const { return m_totals_row_count; }
    pstring get_name() const { return m_name; }
    pstring get_display_name() const { return m_display_name; }
    pstring get_ref() const { return m_ref; }
};

class table_column_attr_parser : public unary_function<xml_token_attr_t, void>
{
    string_pool* m_pool;

    long m_id;
    pstring m_name;
    pstring m_totals_row_label;
    spreadsheet::totals_row_function_t m_totals_row_func;

public:
    table_column_attr_parser(string_pool* pool) :
        m_pool(pool), m_id(-1), m_totals_row_func(spreadsheet::totals_row_function_none) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_id:
                m_id = to_long(attr.value);
            break;
            case XML_name:
                m_name = attr.value;
                if (attr.transient)
                    m_name = m_pool->intern(m_name).first;
            break;
            case XML_totalsRowLabel:
                m_totals_row_label = attr.value;
                if (attr.transient)
                    m_totals_row_label = m_pool->intern(m_totals_row_label).first;
            break;
            case XML_totalsRowFunction:
                m_totals_row_func =
                    spreadsheet::to_totals_row_function_enum(
                        attr.value.get(), attr.value.size());
            break;
            default:
                ;
        }
    }

    long get_id() const { return m_id; }
    pstring get_name() const { return m_name; }
    pstring get_totals_row_label() const { return m_totals_row_label; }
    spreadsheet::totals_row_function_t get_totals_row_function() const { return m_totals_row_func; }
};

}

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
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            table_attr_parser func(&get_session_context().m_string_pool);
            func = for_each(attrs.begin(), attrs.end(), func);

            cout << "* table (range=" << func.get_ref() << "; id=" << func.get_id()
                 << "; name=" << func.get_name() << "; display name="
                 << func.get_display_name() << ")" << endl;
            cout << "  * totals row count: " << func.get_totals_row_count() << endl;
        }
        break;
        case XML_tableColumns:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_table);
            single_long_attr_getter func(NS_ooxml_xlsx, XML_count);
            long column_count = for_each(attrs.begin(), attrs.end(), func).get_value();
            cout << "  * column count: " << column_count << endl;
        }
        break;
        case XML_tableColumn:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_tableColumns);
            table_column_attr_parser func(&get_session_context().m_string_pool);
            func = for_each(attrs.begin(), attrs.end(), func);
            cout << "  * table column (id=" << func.get_id() << "; name=" << func.get_name() << ")" << endl;
            cout << "    * totals row label: " << func.get_totals_row_label() << endl;
            cout << "    * totals func: " << func.get_totals_row_function() << endl;
        }
        break;
        case XML_tableStyleInfo:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_table);

            // TODO : handle this.
        }
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
