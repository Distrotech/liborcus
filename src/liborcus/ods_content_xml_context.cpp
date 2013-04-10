/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

#include "ods_content_xml_context.hpp"
#include "odf_para_context.hpp"
#include "odf_token_constants.hpp"
#include "odf_namespace_types.hpp"

#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

using namespace std;

namespace orcus {

namespace {

class null_date_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    null_date_attr_parser() {}
    null_date_attr_parser(const null_date_attr_parser& r) :
        m_date_value(r.m_date_value)
    {
    }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_table && attr.name == XML_date_value)
            m_date_value = attr.value;
    }

    const pstring& get_date_value() const { return m_date_value; }
private:
    pstring m_date_value;
};

class table_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    table_attr_parser() {}
    table_attr_parser(const table_attr_parser& r) :
        m_name(r.m_name)
    {
    }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_table && attr.name == XML_name)
            m_name = attr.value;
    }

    const pstring& get_name() const { return m_name; }
private:
    pstring m_name;
};

class row_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    row_attr_parser(ods_content_xml_context::row_attr& attr) :
        m_attr(attr) {}
    row_attr_parser(const row_attr_parser& r) :
        m_attr(r.m_attr) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_table && attr.name == XML_number_rows_repeated)
        {
            char* endptr;
            long val = strtol(attr.value.str().c_str(), &endptr, 10);
            if (endptr != attr.value.str())
                m_attr.number_rows_repeated = val;
        }
    }
private:
    ods_content_xml_context::row_attr& m_attr;
};

class cell_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    cell_attr_parser(ods_content_xml_context::cell_attr& attr) :
        m_attr(attr) {}
    cell_attr_parser(const cell_attr_parser& r) :
        m_attr(r.m_attr) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.value.empty())
            return;

        if (attr.ns == NS_odf_table)
            process_ns_table(attr);

        if (attr.ns == NS_odf_office)
            process_ns_office(attr);
    }
private:
    void process_ns_table(const xml_token_attr_t &attr)
    {
        switch (attr.name)
        {
            case XML_number_columns_repeated:
            {
                const char* end = attr.value.get() + attr.value.size();
                char* endptr;
                long val = strtol(attr.value.get(), &endptr, 10);
                if (endptr == end)
                    m_attr.number_columns_repeated = static_cast<int>(val);
            }
            break;
            default:
                ;
        }
    }

    void process_ns_office(const xml_token_attr_t &attr)
    {
        switch (attr.name)
        {
            case XML_value:
            {
                const char* end = attr.value.get() + attr.value.size();
                char* endptr;
                double val = strtod(attr.value.get(), &endptr);
                if (endptr == end)
                    m_attr.value = val;
            }
            break;
            case XML_value_type:
            {
                if (!std::strncmp(attr.value.get(), "float", 5))
                    m_attr.type = ods_content_xml_context::vt_float;
                else if (!std::strncmp(attr.value.get(), "string", 6))
                    m_attr.type = ods_content_xml_context::vt_string;
                else if (!std::strncmp(attr.value.get(), "date", 4))
                    m_attr.type = ods_content_xml_context::vt_date;
            }
            break;
            case XML_date_value:
                m_attr.date_value = attr.value;
            break;
            default:
                ;
        }
    }

    ods_content_xml_context::cell_attr& m_attr;
};

}

// ============================================================================

ods_content_xml_context::row_attr::row_attr() :
    number_rows_repeated(1)
{
}

ods_content_xml_context::cell_attr::cell_attr() :
    number_columns_repeated(1), type(vt_unknown), value(0.0)
{
}

// ============================================================================

ods_content_xml_context::ods_content_xml_context(const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(tokens),
    mp_factory(factory),
    m_row(0), m_col(0),
    m_para_index(0),
    m_has_content(false)
{
}

ods_content_xml_context::~ods_content_xml_context()
{
}

bool ods_content_xml_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_odf_text && name == XML_p)
        return false;

    return true;
}

xml_context_base* ods_content_xml_context::create_child_context(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_odf_text && name == XML_p)
        return new text_para_context(get_tokens(), mp_factory->get_shared_strings());

    return NULL;
}

void ods_content_xml_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
    if (ns == NS_odf_text && name == XML_p)
    {
        text_para_context* para_context = static_cast<text_para_context*>(child);
        m_has_content = !para_context->empty();
        m_para_index = para_context->get_string_index();
    }
}

void ods_content_xml_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    if (ns == NS_odf_office)
    {
        switch (name)
        {
            case XML_body:
            break;
            case XML_spreadsheet:
            break;
            default:
                warn_unhandled();
        }
    }
    else if (ns == NS_odf_table)
    {
        switch (name)
        {
            case XML_calculation_settings:
            break;
            case XML_null_date:
                start_null_date(attrs, parent);
            break;
            case XML_table:
                start_table(attrs, parent);
            break;
            case XML_table_column:
                start_column(attrs, parent);
            break;
            case XML_table_row:
                start_row(attrs, parent);
            break;
            case XML_table_cell:
                start_cell(attrs, parent);
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool ods_content_xml_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_office)
    {
        switch (name)
        {
            case XML_body:
            break;
            case XML_spreadsheet:
            break;
            default:
                ;
        }
    }
    else if (ns == NS_odf_table)
    {
        switch (name)
        {
            case XML_calculation_settings:
            break;
            case XML_null_date:
            break;
            case XML_table:
                end_table();
            break;
            case XML_table_column:
                end_column();
            break;
            case XML_table_row:
                end_row();
            break;
            case XML_table_cell:
                end_cell();
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void ods_content_xml_context::characters(const pstring& str)
{
}

void ods_content_xml_context::start_null_date(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (parent.first != NS_odf_table || parent.second != XML_calculation_settings)
    {
        warn_unexpected();
        return;
    }

    spreadsheet::iface::import_global_settings* gs = mp_factory->get_global_settings();
    if (!gs)
        // Global settings not available. No point going further.
        return;

    pstring null_date = for_each(attrs.begin(), attrs.end(), null_date_attr_parser()).get_date_value();
    date_time_t val = to_date_time(null_date);

    gs->set_origin_date(val.year, val.month, val.day);
}

void ods_content_xml_context::start_table(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (parent.first != NS_odf_office || parent.second != XML_spreadsheet)
    {
        warn_unexpected();
        return;
    }

    table_attr_parser parser = for_each(attrs.begin(), attrs.end(), table_attr_parser());
    const pstring& name = parser.get_name();
    m_tables.push_back(mp_factory->append_sheet(name.get(), name.size()));
    cout << "start table " << name << endl;

    m_row = m_col = 0;
}

void ods_content_xml_context::end_table()
{
    cout << "end table" << endl;
}

void ods_content_xml_context::start_column(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (parent.first == NS_odf_table)
    {
        switch (parent.second)
        {
            case XML_table:
                // TODO: Handle this.
            break;
            default:
                warn_unexpected();
        }
    }
    else
        warn_unexpected();
}

void ods_content_xml_context::end_column()
{
}

void ods_content_xml_context::start_row(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (parent.first == NS_odf_table)
    {
        switch (parent.second)
        {
            case XML_table:
                m_col = 0;
                m_row_attr = row_attr();
                for_each(attrs.begin(), attrs.end(), row_attr_parser(m_row_attr));
            break;
            default:
                warn_unexpected();
        }
    }
    else
        warn_unexpected();

}

void ods_content_xml_context::end_row()
{
    if (m_row_attr.number_rows_repeated > 1)
    {
        // TODO: repeat this row.
//      cout << "repeat this row " << m_row_attr.number_rows_repeated << " times" << endl;
    }
    m_row += m_row_attr.number_rows_repeated;
}

void ods_content_xml_context::start_cell(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (parent.first == NS_odf_table)
    {
        switch (parent.second)
        {
            case XML_table_row:
                m_cell_attr = cell_attr();
                for_each(attrs.begin(), attrs.end(), cell_attr_parser(m_cell_attr));
            break;
            default:
                warn_unexpected();
        }
    }
    else
        warn_unexpected();
}

void ods_content_xml_context::end_cell()
{
    push_cell_value();

    ++m_col;
    if (m_cell_attr.number_columns_repeated > 1)
    {
        int col_upper = m_col + m_cell_attr.number_columns_repeated - 2;
        for (; m_col <= col_upper; ++m_col)
            push_cell_value();
    }
    m_has_content = false;
}

void ods_content_xml_context::push_cell_value()
{
    switch (m_cell_attr.type)
    {
        case vt_float:
            m_tables.back()->set_value(m_row, m_col, m_cell_attr.value);
        break;
        case vt_string:
            if (m_has_content)
                m_tables.back()->set_string(m_row, m_col, m_para_index);
        break;
        case vt_date:
        {
            date_time_t val = to_date_time(m_cell_attr.date_value);
            m_tables.back()->set_date_time(
                m_row, m_col, val.year, val.month, val.day, val.hour, val.minute, val.second);
        }
        break;
        default:
            ;
    }
}

}
