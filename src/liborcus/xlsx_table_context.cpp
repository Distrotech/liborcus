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
#include "orcus/spreadsheet/import_interface.hpp"

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
        m_pool(pool), m_id(-1), m_totals_row_func(spreadsheet::totals_row_function_t::none) {}

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

class table_style_info_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::iface::import_table* mp_table;
    bool m_debug;

public:
    table_style_info_attr_parser(spreadsheet::iface::import_table* table, bool debug) :
        mp_table(table), m_debug(debug) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        bool b = false;

        switch (attr.name)
        {
            case XML_name:
                mp_table->set_style_name(attr.value.get(), attr.value.size());
                if (m_debug)
                    cout << "  * table style info (name=" << attr.value << ")" << endl;
            break;
            case XML_showFirstColumn:
                b = to_bool(attr.value);
                mp_table->set_style_show_first_column(b);
                if (m_debug)
                    cout << "    * show first column: " << b << endl;
            break;
            case XML_showLastColumn:
                b = to_bool(attr.value);
                mp_table->set_style_show_last_column(b);
                if (m_debug)
                    cout << "    * show last column: " << b << endl;
            break;
            case XML_showRowStripes:
                b = to_bool(attr.value);
                mp_table->set_style_show_row_stripes(b);
                if (m_debug)
                    cout << "    * show row stripes: " << b << endl;
            break;
            case XML_showColumnStripes:
                b = to_bool(attr.value);
                mp_table->set_style_show_column_stripes(b);
                if (m_debug)
                    cout << "    * show column stripes: " << b << endl;
            break;
            default:
                ;
        }
    }
};

}

xlsx_table_context::xlsx_table_context(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_table& table) :
    xml_context_base(session_cxt, tokens), m_table(table) {}

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
        spreadsheet::iface::import_auto_filter* af = m_table.get_auto_filter();
        if (!af)
            return;

        const xlsx_autofilter_context& cxt = static_cast<const xlsx_autofilter_context&>(*child);
        cxt.push_to_model(*af);
    }
}

void xlsx_table_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns != NS_ooxml_xlsx)
        return;

    pstring str;

    switch (name)
    {
        case XML_table:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            table_attr_parser func(&get_session_context().m_string_pool);
            func = for_each(attrs.begin(), attrs.end(), func);

            if (get_config().debug)
            {
                cout << "* table (range=" << func.get_ref() << "; id=" << func.get_id()
                     << "; name=" << func.get_name() << "; display name="
                     << func.get_display_name() << ")" << endl;
                cout << "  * totals row count: " << func.get_totals_row_count() << endl;
            }

            m_table.set_identifier(func.get_id());
            str = func.get_ref();
            m_table.set_range(str.get(), str.size());
            str = func.get_name();
            m_table.set_name(str.get(), str.size());
            str = func.get_display_name();
            m_table.set_display_name(str.get(), str.size());
            m_table.set_totals_row_count(func.get_totals_row_count());
        }
        break;
        case XML_tableColumns:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_table);
            single_long_attr_getter func(NS_ooxml_xlsx, XML_count);
            long column_count = for_each(attrs.begin(), attrs.end(), func).get_value();
            if (get_config().debug)
                cout << "  * column count: " << column_count << endl;

            m_table.set_column_count(column_count);
        }
        break;
        case XML_tableColumn:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_tableColumns);
            table_column_attr_parser func(&get_session_context().m_string_pool);
            func = for_each(attrs.begin(), attrs.end(), func);
            if (get_config().debug)
            {
                cout << "  * table column (id=" << func.get_id() << "; name=" << func.get_name() << ")" << endl;
                cout << "    * totals row label: " << func.get_totals_row_label() << endl;
                cout << "    * totals func: " << static_cast<int>(func.get_totals_row_function()) << endl;
            }

            m_table.set_column_identifier(func.get_id());
            str = func.get_name();
            m_table.set_column_name(str.get(), str.size());
            str = func.get_totals_row_label();
            m_table.set_column_totals_row_label(str.get(), str.size());
            m_table.set_column_totals_row_function(func.get_totals_row_function());
        }
        break;
        case XML_tableStyleInfo:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_table);
            table_style_info_attr_parser func(&m_table, get_config().debug);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        default:
            warn_unhandled();
    }

}

bool xlsx_table_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_table:
                m_table.commit();
            break;
            case XML_tableColumn:
                m_table.commit_column();
            break;
            default:
                ;
        }
    }

    return pop_stack(ns, name);
}

void xlsx_table_context::characters(const pstring& str, bool transient)
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
