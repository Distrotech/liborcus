/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xls_xml_context.hpp"
#include "xls_xml_namespace_types.hpp"
#include "xls_xml_token_constants.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/measurement.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

class sheet_attr_parser : public unary_function<xml_token_attr_t, void>
{
    pstring m_name;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_xls_xml_ss)
        {
            switch (attr.name)
            {
                case XML_Name:
                    m_name = attr.value;
                break;
                default:
                    ;
            }
        }
    }

    pstring get_name() const { return m_name; }
};

class data_attr_parser : public unary_function<xml_token_attr_t, void>
{
    xls_xml_context::cell_type m_type;

public:
    data_attr_parser() : m_type(xls_xml_context::ct_unknown) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_xls_xml_ss)
        {
            switch (attr.name)
            {
                case XML_Type:
                {
                    if (attr.value == "String")
                        m_type = xls_xml_context::ct_string;
                    else if (attr.value == "Number")
                        m_type = xls_xml_context::ct_number;

                }
                break;
                default:
                    ;
            }
        }
    }

    xls_xml_context::cell_type get_cell_type() const { return m_type; }
};

class row_attr_parser : public unary_function<xml_token_attr_t, void>
{
    long m_row_index;
public:
    row_attr_parser() : m_row_index(-1) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.value.empty())
            return;

        if (attr.ns == NS_xls_xml_ss)
        {
            switch (attr.name)
            {
                case XML_Index:
                {
                    const char* p = attr.value.get();
                    const char* p_end = p + attr.value.size();
                    m_row_index = to_long(p, p_end);
                }
                break;
                default:
                    ;
            }
        }
    }

    long get_row_index() const { return m_row_index; }
};

class cell_attr_parser : public unary_function<xml_token_attr_t, void>
{
    long m_col_index;
public:
    cell_attr_parser() : m_col_index(-1) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.value.empty())
            return;

        if (attr.ns == NS_xls_xml_ss)
        {
            switch (attr.name)
            {
                case XML_Index:
                {
                    const char* p = attr.value.get();
                    const char* p_end = p + attr.value.size();
                    m_col_index = to_long(p, p_end);
                }
                break;
                default:
                    ;
            }
        }
    }

    long get_col_index() const { return m_col_index; }
};


}

xls_xml_context::xls_xml_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory),
    mp_cur_sheet(nullptr),
    m_cur_row(0), m_cur_col(0), m_cur_cell_type(ct_unknown),
    m_cur_cell_value(std::numeric_limits<double>::quiet_NaN())
{
}

xls_xml_context::~xls_xml_context()
{
}

bool xls_xml_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xls_xml_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return nullptr;
}

void xls_xml_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xls_xml_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_xls_xml_ss)
    {
        switch (name)
        {
            case XML_Workbook:
                // Do nothing.
            break;
            case XML_Worksheet:
            {
                xml_element_expected(parent, NS_xls_xml_ss, XML_Workbook);
                pstring sheet_name = for_each(attrs.begin(), attrs.end(), sheet_attr_parser()).get_name();
                mp_cur_sheet = mp_factory->append_sheet(sheet_name.get(), sheet_name.size());
                m_cur_row = 0;
                m_cur_col = 0;
            }
            break;
            case XML_Table:
                xml_element_expected(parent, NS_xls_xml_ss, XML_Worksheet);
            break;
            case XML_Row:
            {
                xml_element_expected(parent, NS_xls_xml_ss, XML_Table);
                m_cur_col = 0;
                long row_index = for_each(attrs.begin(), attrs.end(), row_attr_parser()).get_row_index();
                if (row_index > 0)
                {
                    // 1-based row index. Convert it to a 0-based one.
                    m_cur_row = row_index - 1;
                }
            }
            break;
            case XML_Cell:
            {
                xml_element_expected(parent, NS_xls_xml_ss, XML_Row);
                long col_index = for_each(attrs.begin(), attrs.end(), cell_attr_parser()).get_col_index();
                if (col_index > 0)
                {
                    // 1-based column index. Convert it to a 0-based one.
                    m_cur_col = col_index - 1;
                }
            }
            break;
            case XML_Data:
                xml_element_expected(parent, NS_xls_xml_ss, XML_Cell);
                m_cur_cell_type = for_each(attrs.begin(), attrs.end(), data_attr_parser()).get_cell_type();
                m_cur_cell_string.clear();
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool xls_xml_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_xls_xml_ss)
    {
        switch (name)
        {
            case XML_Row:
                ++m_cur_row;
            break;
            case XML_Cell:
                ++m_cur_col;
            break;
            case XML_Data:
                push_cell();
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void xls_xml_context::characters(const pstring& str, bool transient)
{
    if (str.empty())
        return;

    const xml_token_pair_t& elem = get_current_element();

    if (elem.first == NS_xls_xml_ss && elem.second == XML_Data)
    {
        switch (m_cur_cell_type)
        {
            case ct_string:
            {
                if (transient)
                    m_cur_cell_string.push_back(m_pool.intern(str).first);
                else
                    m_cur_cell_string.push_back(str);
            }
            break;
            case ct_number:
            {
                const char* p = str.get();
                m_cur_cell_value = to_double(p, p + str.size());
            }
            break;
            default:
                ;
        }
    }
}

void xls_xml_context::push_cell()
{
    switch (m_cur_cell_type)
    {
        case ct_number:
            mp_cur_sheet->set_value(m_cur_row, m_cur_col, m_cur_cell_value);
        break;
        case ct_string:
        {
            spreadsheet::iface::import_shared_strings* ss = mp_factory->get_shared_strings();
            if (!ss)
                return;

            if (m_cur_cell_string.empty())
                return;

            if (m_cur_cell_string.size() == 1)
            {
                const pstring& s = m_cur_cell_string.back();
                mp_cur_sheet->set_string(m_cur_row, m_cur_col, ss->append(&s[0], s.size()));
            }
            else
            {
                string s;
                vector<pstring>::const_iterator it = m_cur_cell_string.begin(), it_end = m_cur_cell_string.end();
                for (; it != it_end; ++it)
                    s += *it;

                mp_cur_sheet->set_string(m_cur_row, m_cur_col, ss->append(&s[0], s.size()));
            }
            m_cur_cell_string.clear();
        }
        break;
        default:
            ;
    }
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
