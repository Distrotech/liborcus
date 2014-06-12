/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ods_content_xml_context.hpp"
#include "odf_token_constants.hpp"
#include "odf_namespace_types.hpp"
#include "odf_styles_context.hpp"
#include "session_context.hpp"
#include "ods_session_data.hpp"

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

class column_attr_parser : public unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_table)
        {
            if (attr.name == XML_style_name)
                m_style_name = attr.value;
        }
    }

    const pstring& get_style_name() const { return m_style_name; }
};

class row_attr_parser : public unary_function<xml_token_attr_t, void>
{
    int m_number_rows_repeated;
    pstring m_style_name;

public:
    row_attr_parser() : m_number_rows_repeated(1) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_table)
        {
            switch (attr.name)
            {
                case XML_number_rows_repeated:
                {
                    char* endptr;
                    long val = strtol(attr.value.str().c_str(), &endptr, 10);
                    if (endptr != attr.value.str())
                        m_number_rows_repeated = val;
                }
                break;
                case XML_style_name:
                    m_style_name = attr.value;
                break;
            }
        }
    }

    int get_number_rows_repeated() const { return m_number_rows_repeated; }
    const pstring& get_style_name() const { return m_style_name; }
};

class cell_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    cell_attr_parser(session_context& cxt, ods_content_xml_context::cell_attr& attr) :
        m_cxt(cxt), m_attr(attr) {}
    cell_attr_parser(const cell_attr_parser& r) :
        m_cxt(r.m_cxt), m_attr(r.m_attr) {}

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

    void process_formula(const pstring& str, bool transient)
    {
        if (str.empty())
            return;

        // Formula is prefixed with formula type, followed by a ':'
        // then the actual formula content.

        // First, detect prefix if any.  Only try up to the first 5 characters.
        const char* p0 = str.get();
        const char* end = p0 + std::min<size_t>(str.size(), 5);
        size_t prefix_size = 0;
        for (const char* p = p0; p != end; ++p)
        {
            if (*p == ':')
            {
                // Prefix separator found.
                prefix_size = p - p0;
                break;
            }

            if (!is_alpha(*p))
                // Only alphabets are allowed in the prefix space.
                break;
        }

        pstring prefix, formula;
        if (prefix_size)
        {
            prefix = pstring(p0, prefix_size);
            const char* p = p0;
            p += prefix_size + 1;
            end = p0 + str.size();
            formula = pstring(p, end - p);
        }
        else
        {
            // TODO : Handle cases where a formula doesn't have a prefix.
        }

        // Formula needs to begin with '='.
        if (formula.empty() || formula[0] != '=')
            return;

        // Remove the '='.
        formula = pstring(formula.get()+1, formula.size()-1);

        if (prefix == "of")
        {
            // ODF formula.  No action needed.
        }

        m_attr.formula = formula;
        if (transient)
            m_attr.formula = m_cxt.m_string_pool.intern(m_attr.formula).first;
    }

    void process_ns_table(const xml_token_attr_t &attr)
    {
        switch (attr.name)
        {
            case XML_style_name:
                m_attr.style_name = attr.value;
            break;
            case XML_number_columns_repeated:
            {
                const char* end = attr.value.get() + attr.value.size();
                char* endptr;
                long val = strtol(attr.value.get(), &endptr, 10);
                if (endptr == end)
                    m_attr.number_columns_repeated = static_cast<int>(val);
            }
            break;
            case XML_formula:
                process_formula(attr.value, attr.transient);
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

    session_context& m_cxt;
    ods_content_xml_context::cell_attr& m_attr;
};

}

// ============================================================================

ods_content_xml_context::row_attr::row_attr() :
    number_rows_repeated(1)
{
}

ods_content_xml_context::cell_attr::cell_attr() :
    number_columns_repeated(1),
    type(vt_unknown),
    value(0.0),
    formula_grammar(spreadsheet::formula_grammar_ods)
{
}

// ============================================================================

ods_content_xml_context::ods_content_xml_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory),
    m_row(0), m_col(0),
    m_para_index(0),
    m_has_content(false),
    m_styles(),
    m_child_para(session_cxt, tokens, factory->get_shared_strings(), m_styles)
{
}

ods_content_xml_context::~ods_content_xml_context()
{
}

bool ods_content_xml_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_odf_text && name == XML_p)
        return false;

    if (ns == NS_odf_office && name == XML_automatic_styles)
        return false;

    return true;
}

xml_context_base* ods_content_xml_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_text && name == XML_p)
    {
        m_child_para.reset();
        return &m_child_para;
    }

    if (ns == NS_odf_office && name == XML_automatic_styles)
    {
        mp_child.reset(new automatic_styles_context(get_session_context(), get_tokens(), m_styles, mp_factory));
        return mp_child.get();
    }

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
    else if (ns == NS_odf_office && name == XML_automatic_styles)
    {
        cout << "styles picked up:" << endl;
        odf_styles_map_type::const_iterator it = m_styles.begin(), it_end = m_styles.end();
        for (; it != it_end; ++it)
        {
            cout << "  style: " << it->first << " [ ";
            switch (it->second->family)
            {
                case style_family_table_column:
                    cout << "column width: " << it->second->column_data->width.print();
                break;
                case style_family_table_row:
                    cout << "row height: " << it->second->row_data->height.print();
                break;
                case style_family_table_cell:
                {
                    const odf_style::cell& cell = *it->second->cell_data;
                    cout << "font ID: " << cell.font;
                    spreadsheet::iface::import_styles* styles = mp_factory->get_styles();
                    if (styles)
                    {
                        styles->set_xf_font(cell.font);
                        size_t xf_id = styles->commit_cell_xf();
                        m_cell_format_map.insert(name2id_type::value_type(it->first, xf_id));
                    }
                }
                break;
                case style_family_text:
                {
                    const odf_style::text& data = *it->second->text_data;
                    cout << "font ID: " << data.font;
                }
                break;
                default:
                    ;
            }
            cout << " ]" << endl;
        }
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
                xml_element_expected(parent, NS_odf_table, XML_calculation_settings);
                start_null_date(attrs);
            break;
            case XML_table:
                xml_element_expected(parent, NS_odf_office, XML_spreadsheet);
                start_table(attrs);
            break;
            case XML_table_column:
                xml_element_expected(parent, NS_odf_table, XML_table);
                start_column(attrs);
            break;
            case XML_table_row:
                xml_element_expected(parent, NS_odf_table, XML_table);
                start_row(attrs);
            break;
            case XML_table_cell:
                xml_element_expected(parent, NS_odf_table, XML_table_row);
                start_cell(attrs);
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
                end_spreadsheet();
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

void ods_content_xml_context::characters(const pstring& str, bool transient)
{
}

void ods_content_xml_context::start_null_date(const xml_attrs_t& attrs)
{
    spreadsheet::iface::import_global_settings* gs = mp_factory->get_global_settings();
    if (!gs)
        // Global settings not available. No point going further.
        return;

    pstring null_date = for_each(attrs.begin(), attrs.end(), null_date_attr_parser()).get_date_value();
    date_time_t val = to_date_time(null_date);

    gs->set_origin_date(val.year, val.month, val.day);
}

void ods_content_xml_context::start_table(const xml_attrs_t& attrs)
{
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

void ods_content_xml_context::start_column(const xml_attrs_t& attrs)
{
    spreadsheet::iface::import_sheet_properties* sheet_props =
        m_tables.back()->get_sheet_properties();

    if (!sheet_props)
        return;

    column_attr_parser func;
    func = for_each(attrs.begin(), attrs.end(), func);

    odf_styles_map_type::const_iterator it = m_styles.find(func.get_style_name());
    if (it == m_styles.end())
        // Style by this name not found.
        return;

    const odf_style& style = *it->second;
    sheet_props->set_column_width(m_col, style.column_data->width.value, style.column_data->width.unit);
}

void ods_content_xml_context::end_column()
{
    ++m_col;
}

void ods_content_xml_context::start_row(const xml_attrs_t& attrs)
{
    m_col = 0;
    m_row_attr = row_attr();
    row_attr_parser func;
    func = for_each(attrs.begin(), attrs.end(), func);
    m_row_attr.number_rows_repeated = func.get_number_rows_repeated();

    // Pass row properties to the interface.
    spreadsheet::iface::import_sheet_properties* sheet_props =
        m_tables.back()->get_sheet_properties();

    if (sheet_props)
    {
        odf_styles_map_type::const_iterator it = m_styles.find(func.get_style_name());
        if (it != m_styles.end())
        {
            const odf_style::row& row_data = *it->second->row_data;
            sheet_props->set_row_height(m_row, row_data.height.value, row_data.height.unit);
        }
    }
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

void ods_content_xml_context::start_cell(const xml_attrs_t& attrs)
{
    m_cell_attr = cell_attr();
    for_each(attrs.begin(), attrs.end(), cell_attr_parser(get_session_context(), m_cell_attr));
}

void ods_content_xml_context::end_cell()
{
    name2id_type::const_iterator it = m_cell_format_map.find(m_cell_attr.style_name);
    if (it != m_cell_format_map.end())
        m_tables.back()->set_format(m_row, m_col, it->second);

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
    spreadsheet::iface::import_sheet* sheet = m_tables.back();
    bool has_formula = !m_cell_attr.formula.empty();
    if (has_formula)
    {
        ods_session_data& ods_data =
            static_cast<ods_session_data&>(*get_session_context().mp_data);
        ods_data.m_formulas.push_back(
            new ods_session_data::formula(
                m_tables.size()-1, m_row, m_col, m_cell_attr.formula_grammar, m_cell_attr.formula));

        ods_session_data::formula& formula_data = ods_data.m_formulas.back();

        // Store formula result.
        switch (m_cell_attr.type)
        {
            case vt_float:
            {
                formula_data.result.type = orcus::ods_session_data::rt_numeric;
                formula_data.result.numeric_value = m_cell_attr.value;
            }
            break;
            case vt_string:
                // TODO : pass string result here.  We need to decide whether
                // to pass a string ID or a raw string.
            break;
            default:
                ;
        }
        return;
    }

    switch (m_cell_attr.type)
    {
        case vt_float:
            sheet->set_value(m_row, m_col, m_cell_attr.value);
        break;
        case vt_string:
            if (m_has_content)
                sheet->set_string(m_row, m_col, m_para_index);
        break;
        case vt_date:
        {
            date_time_t val = to_date_time(m_cell_attr.date_value);
            sheet->set_date_time(
                m_row, m_col, val.year, val.month, val.day, val.hour, val.minute, val.second);
        }
        break;
        default:
            ;
    }
}

void ods_content_xml_context::end_spreadsheet()
{
    ods_session_data& ods_data =
        static_cast<ods_session_data&>(*get_session_context().mp_data);

    // Push all formula cells.  Formula cells needs to be processed after all
    // the sheet data have been imported, else 3D reference would fail to
    // resolve.
    ods_session_data::formulas_type::iterator it = ods_data.m_formulas.begin(), ite = ods_data.m_formulas.end();
    for (; it != ite; ++it)
    {
        ods_session_data::formula& data = *it;
        if (data.sheet < 0 || static_cast<size_t>(data.sheet) >= m_tables.size())
            // Invalid sheet index.
            continue;

        spreadsheet::iface::import_sheet* sheet = m_tables[data.sheet];
        sheet->set_formula(
            data.row, data.column, data.grammar,
            data.exp.get(), data.exp.size());

        switch (data.result.type)
        {
            case ods_session_data::rt_numeric:
                sheet->set_formula_result(data.row, data.column, data.result.numeric_value);
            break;
            case ods_session_data::rt_string:
            case ods_session_data::rt_error:
            case ods_session_data::rt_none:
            default:
                ;
        }
    }

    // Clear the formula buffer.
    ods_data.m_formulas.clear();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
