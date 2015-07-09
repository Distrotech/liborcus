/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gnumeric_sheet_context.hpp"
#include "gnumeric_cell_context.hpp"
#include "gnumeric_token_constants.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_helper.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "gnumeric_helper.hpp"

namespace orcus {

namespace {

class gnumeric_style_region_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_style_region_attr_parser(gnumeric_style_region& style_region_data):
        m_style_region_data(style_region_data) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_startCol:
            {
                size_t n = atoi(attr.value.get());
                m_style_region_data.start_col = n;
            }
            break;
            case XML_startRow:
            {
                size_t n = atoi(attr.value.get());
                m_style_region_data.start_row = n;
            }
            break;
            case XML_endCol:
            {
                size_t n = atoi(attr.value.get());
                m_style_region_data.end_col = n;
            }
            break;
            case XML_endRow:
            {
                size_t n = atoi(attr.value.get());
                m_style_region_data.end_row = n;
            }
            break;
            default:
                ;
        }
    }

private:
    gnumeric_style_region& m_style_region_data;
};

class gnumeric_font_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_font_attr_parser(spreadsheet::iface::import_styles& styles) :
        m_styles(styles) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_Unit:
            {
                double n = atoi(attr.value.get());
                m_styles.set_font_size(n);
            }
            break;
            case XML_Bold:
            {
                bool b = atoi(attr.value.get()) != 0;
                m_styles.set_font_bold(b);
            }
            break;
            case XML_Italic:
            {
                bool b = atoi(attr.value.get()) != 0;
                m_styles.set_font_italic(b);
            }
            break;
            case XML_Underline:
            {
                int n = atoi(attr.value.get());
                switch (n)
                {
                    case 0:
                        m_styles.set_font_underline(spreadsheet::underline_t::none);
                    break;
                    case 1:
                        m_styles.set_font_underline(spreadsheet::underline_t::single_line);
                    break;
                    case 2:
                        m_styles.set_font_underline(spreadsheet::underline_t::double_line);
                    break;
                    default:
                        ;
                }
            }
            break;
        }
    }

private:
    spreadsheet::iface::import_styles& m_styles;
};

class gnumeric_style_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_style_attr_parser(spreadsheet::iface::import_styles& styles, gnumeric_color& front_color) :
        m_styles(styles),
        m_fill(false),
        m_protection(false),
        m_front_color(front_color) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_Fore:
            {
                spreadsheet::color_elem_t red, green, blue;
                gnumeric_helper::parse_RGB_color_attribute(red, green, blue, attr.value);
                m_styles.set_fill_fg_color(0, red, green, blue);

                m_fill = true;

                m_front_color.red = red;
                m_front_color.blue = blue;
                m_front_color.green = green;
            }
            break;
            case XML_Back:
            {
                spreadsheet::color_elem_t red, green, blue;
                gnumeric_helper::parse_RGB_color_attribute(red, green, blue, attr.value);
                m_styles.set_fill_bg_color(0, red, green, blue);

                m_fill = true;
            }
            break;
            case XML_Hidden:
            {
                bool b = atoi(attr.value.get());
                m_styles.set_cell_hidden(b);

                m_protection = true;
            }
            break;
            case XML_Locked:
            {
                bool b = atoi(attr.value.get());
                m_styles.set_cell_locked(b);

                m_protection = true;
            }
            break;
            case XML_Format:
            {
                if (attr.value != "General")
                {
                    m_styles.set_number_format_code(attr.value.get(), attr.value.size());
                    size_t index = m_styles.commit_number_format();
                    m_styles.set_xf_number_format(index);
                }
            }
            break;
            case XML_HAlign:
            {
                orcus::spreadsheet::hor_alignment_t hor_alignment = orcus::spreadsheet::hor_alignment_t::unknown;
                if (attr.value == "GNM_HALIGN_CENTER")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::center;
                else if (attr.value == "GNM_HALIGN_RIGHT")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::right;
                else if (attr.value == "GNM_HALIGN_LEFT")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::left;
                else if (attr.value == "GNM_HALIGN_JUSTIFY")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::justified;
                else if (attr.value == "GNM_HALIGN_DISTRIBUTED")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::distributed;
                else if (attr.value == "GNM_HALIGN_FILL")
                    hor_alignment = orcus::spreadsheet::hor_alignment_t::filled;

                if (hor_alignment != orcus::spreadsheet::hor_alignment_t::unknown)
                    m_styles.set_xf_apply_alignment(true);
                m_styles.set_xf_horizontal_alignment(hor_alignment);
            }
            break;
            case XML_VAlign:
            {
                orcus::spreadsheet::ver_alignment_t ver_alignment = orcus::spreadsheet::ver_alignment_t::unknown;
                if (attr.value == "GNM_VALIGN_BOTTOM")
                    ver_alignment = orcus::spreadsheet::ver_alignment_t::bottom;
                else if (attr.value == "GNM_VALIGN_TOP")
                    ver_alignment = orcus::spreadsheet::ver_alignment_t::top;
                else if (attr.value == "GNM_VALIGN_CENTER")
                    ver_alignment = orcus::spreadsheet::ver_alignment_t::middle;
                else if (attr.value == "GNM_VALIGN_JUSTIFY")
                    ver_alignment = orcus::spreadsheet::ver_alignment_t::justified;
                else if (attr.value == "GNM_VALIGN_DISTRIBUTED")
                    ver_alignment = orcus::spreadsheet::ver_alignment_t::distributed;

                if (ver_alignment != orcus::spreadsheet::ver_alignment_t::unknown)
                    m_styles.set_xf_apply_alignment(true);
                m_styles.set_xf_vertical_alignment(ver_alignment);
            }
            break;
        }
    }

    bool is_protection_set() const
    {
        return m_protection;
    }

    bool is_fill_set() const
    {
        return m_fill;
    }

private:
    spreadsheet::iface::import_styles& m_styles;

    bool m_fill;
    bool m_protection;

    gnumeric_color& m_front_color;
};

spreadsheet::condition_operator_t get_condition_operator(int val)
{
    switch(val)
    {
        case 0:
            return spreadsheet::condition_operator_t::between;
        case 1:
            return spreadsheet::condition_operator_t::not_between;
        case 2:
            return spreadsheet::condition_operator_t::equal;
        case 3:
            return spreadsheet::condition_operator_t::not_equal;
        case 4:
            return spreadsheet::condition_operator_t::greater;
        case 5:
            return spreadsheet::condition_operator_t::less;
        case 6:
            return spreadsheet::condition_operator_t::greater_equal;
        case 7:
            return spreadsheet::condition_operator_t::less_equal;
        case 8:
            return spreadsheet::condition_operator_t::expression;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            break;
        case 16:
            return spreadsheet::condition_operator_t::contains;
        case 17:
            return spreadsheet::condition_operator_t::not_contains;
        case 18:
            return spreadsheet::condition_operator_t::begins_with;
        case 19:
            break;
        case 20:
            return spreadsheet::condition_operator_t::ends_with;
        case 21:
            break;
        case 22:
            return spreadsheet::condition_operator_t::contains_error;
        case 23:
            return spreadsheet::condition_operator_t::contains_no_error;
        default:
            break;
    }
    return orcus::spreadsheet::condition_operator_t::unknown;
}

class gnumeric_condition_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_condition_attr_parser(spreadsheet::iface::import_conditional_format* cond_format):
        m_cond_format(cond_format) {}

    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_Operator:
            {
                int val = atoi(attr.value.get());
                spreadsheet::condition_operator_t op = get_condition_operator(val);
                m_cond_format->set_operator(op);
            }
            break;
            default:
            break;
        }
    }

private:
    spreadsheet::iface::import_conditional_format* m_cond_format;
};

class gnumeric_col_row_info : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_col_row_info() :
        m_position(0),
        m_num_repeated(1),
        m_size(0.0),
        m_hidden(false) {}

    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_No:
            {
                size_t i = atoi(attr.value.get());
                m_position = i;
            }
            break;
            case XML_Unit:
            {
                double n = atof(attr.value.get());
                m_size = n;
            }
            break;
            case XML_Count:
            {
                size_t i = atoi(attr.value.get());
                m_num_repeated = i;
            }
            break;
            case XML_Hidden:
            {
                bool b = atoi(attr.value.get()) != 0;
                m_hidden = b;
            }
        }
    }

    size_t get_position() const
    {
        return m_position;
    }

    size_t get_col_row_repeated() const
    {
        return m_num_repeated;
    }

    double get_size() const
    {
        return m_size;
    }

    bool is_hidden() const
    {
        return m_hidden;
    }

private:
    size_t m_position;
    size_t m_num_repeated;
    double m_size;
    bool m_hidden;

};

class gnumeric_autofilter_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_autofilter_attr_parser(spreadsheet::iface::import_auto_filter& auto_filter):
        m_auto_filter(auto_filter) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_Area:
                m_auto_filter.set_range(attr.value.get(), attr.value.size());
            break;
            default:
                ;
        }
    }

private:
    spreadsheet::iface::import_auto_filter& m_auto_filter;
};

enum gnumeric_filter_field_op_t
{
    filter_equal,
    filter_greaterThan,
    filter_lessThan,
    filter_greaterThanEqual,
    filter_lessThanEqual,
    filter_notEqual,
    filter_op_invalid
};

enum gnumeric_filter_field_type_t
{
    filter_expr,
    filter_blanks,
    filter_nonblanks,
    filter_type_invalid
};

class gnumeric_autofilter_field_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_autofilter_field_attr_parser(spreadsheet::iface::import_auto_filter& auto_filter):
        m_auto_filter(auto_filter),
        m_filter_field_type(filter_type_invalid),
        m_filter_op(filter_op_invalid) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_Index:
            {
                spreadsheet::col_t col = atoi(attr.value.get());                
                m_auto_filter.set_column(col);
            }
            break;
            case XML_Type:
            {
                if (attr.value == "expr")
                    m_filter_field_type = filter_expr;
                else if (attr.value == "blanks")
                    m_filter_field_type = filter_blanks;
                else if (attr.value == "nonblanks")
                    m_filter_field_type = filter_nonblanks;
            }
            break;
            case XML_Op0:
            {
                if (attr.value == "eq")
                    m_filter_op = filter_equal;
                else if (attr.value == "gt")
                    m_filter_op = filter_greaterThan;
                else if (attr.value == "lt")
                    m_filter_op = filter_lessThan;
                else if (attr.value == "gte")
                    m_filter_op = filter_greaterThanEqual;
                else if (attr.value == "lte")
                    m_filter_op = filter_lessThanEqual;
                else if (attr.value == "ne")
                    m_filter_op = filter_notEqual;
            }
            break;
            case XML_Value0:
            {
                m_filter_value_type = attr.value;
            }
            break;
            case XML_ValueType0:
            {
                m_filter_value = attr.value;
            }
            break;
            default:
                ;
        }
    }

    void finalize_filter_import()
    {
        switch (m_filter_field_type)
        {
            case filter_expr:
                import_expr();
                break;
            case filter_type_invalid:
                break;
            default:
                break;
        }
    }

private:

    void import_expr()
    {
        // only equal supported in API yet
        if (m_filter_op != filter_equal)
            return;

        // import condition for integer (30), double(40) and string (60)
        if (m_filter_value_type == "30" ||
                m_filter_value_type == "40" ||
                m_filter_value_type == "60" )
        {
            m_auto_filter.append_column_match_value(m_filter_value.get(), m_filter_value.size());
        }
    }

    spreadsheet::iface::import_auto_filter& m_auto_filter;

    gnumeric_filter_field_type_t m_filter_field_type;
    gnumeric_filter_field_op_t m_filter_op;

    pstring m_filter_value_type;
    pstring m_filter_value;
};

}


gnumeric_sheet_context::gnumeric_sheet_context(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory),
    mp_sheet(NULL),
    mp_auto_filter(NULL)
{
}

gnumeric_sheet_context::~gnumeric_sheet_context()
{
}

bool gnumeric_sheet_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_gnumeric_gnm && name == XML_Cells)
        return false;

    return true;
}

xml_context_base* gnumeric_sheet_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_gnumeric_gnm && name == XML_Cells)
    {
        mp_child.reset(new gnumeric_cell_context(get_session_context(), get_tokens(), mp_factory, mp_sheet));
        mp_child->transfer_common(*this);
        return mp_child.get();
    }

    return NULL;
}

void gnumeric_sheet_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void gnumeric_sheet_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_gnumeric_gnm)
    {
        switch (name)
        {
            case XML_Font:
                start_font(attrs);
            break;
            case XML_Style:
                start_style(attrs);
            break;
            case XML_StyleRegion:
                start_style_region(attrs);
            break;
            case XML_ColInfo:
                start_col(attrs);
            break;
            case XML_RowInfo:
                start_row(attrs);
            break;
            case XML_Filters:
                // don't need any special handling
            break;
            case XML_Filter:
            {
                mp_auto_filter = mp_sheet->get_auto_filter();
                if (mp_auto_filter)
                {
                    std::for_each(attrs.begin(), attrs.end(),
                            gnumeric_autofilter_attr_parser(
                                *mp_auto_filter));
                }
            }
            break;
            case XML_Field:
            {
                assert(parent.first == NS_gnumeric_gnm && parent.second == XML_Filter);
                if (mp_auto_filter)
                {
                    gnumeric_autofilter_field_attr_parser parser =
                        std::for_each(attrs.begin(), attrs.end(),
                                gnumeric_autofilter_field_attr_parser(
                                    *mp_auto_filter));
                    parser.finalize_filter_import();
                }
            }
            break;
            case XML_Condition:
            {
                if (!mp_region_data->contains_conditional_format)
                {
                    mp_region_data->contains_conditional_format = true;
                    end_style(false);
                }
                start_condition(attrs);
            }
            break;
            case XML_Expression0:
            case XML_Expression1:
            break;
            default:
                ;
        }
    }
}

bool gnumeric_sheet_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_gnumeric_gnm)
    {
        switch(name)
        {
            case XML_Name:
            {
                xml_token_pair_t parent = get_parent_element();
                if(parent.first == NS_gnumeric_gnm && parent.second == XML_Sheet)
                    end_table();
                else
                    warn_unhandled();
            }
            break;
            case XML_Font:
                end_font();
            break;
            case XML_Style:
            {
                xml_token_pair_t parent = get_parent_element();
                if (parent.second == XML_Condition)
                {
                    end_style(true);
                }
                else
                {
                    // The conditional format entry contains a mandatory style element
                    // Therefore when we have a conditional format the end_style method
                    // is already called in start_element of the XML_Condition case.
                    if (!mp_region_data->contains_conditional_format)
                    {
                        end_style(false);
                    }
                }
            }
            break;
            case XML_Filter:
                if (mp_auto_filter)
                    mp_auto_filter->commit();
            break;
            case XML_Field:
                if (mp_auto_filter)
                    mp_auto_filter->commit_column();
            break;
            case XML_StyleRegion:
                end_style_region();
            break;
            case XML_Condition:
                end_condition();
            break;
            case XML_Expression0:
            case XML_Expression1:
                end_expression();
            break;
            default:
                ;
        }
    }

    return pop_stack(ns, name);
}

void gnumeric_sheet_context::characters(const pstring& str, bool transient)
{
    chars = str;
}

void gnumeric_sheet_context::start_font(const xml_attrs_t& attrs)
{
    for_each(attrs.begin(), attrs.end(), gnumeric_font_attr_parser(*mp_factory->get_styles()));
}

void gnumeric_sheet_context::start_col(const xml_attrs_t& attrs)
{
    gnumeric_col_row_info col_info = for_each(attrs.begin(), attrs.end(),
            gnumeric_col_row_info());
    spreadsheet::iface::import_sheet_properties* p_sheet_props = mp_sheet->get_sheet_properties();
    double col_size = col_info.get_size();
    bool hidden = col_info.is_hidden();
    for (size_t i = col_info.get_position(),
            n = col_info.get_col_row_repeated() + col_info.get_position(); i < n; ++i)
    {
        p_sheet_props->set_column_width(i, col_size, length_unit_t::point);
        p_sheet_props->set_column_hidden(i, hidden);
    }
}

void gnumeric_sheet_context::start_row(const xml_attrs_t& attrs)
{
    gnumeric_col_row_info row_info = for_each(attrs.begin(), attrs.end(),
            gnumeric_col_row_info());
    spreadsheet::iface::import_sheet_properties* p_sheet_props = mp_sheet->get_sheet_properties();
    double row_size = row_info.get_size();
    bool hidden = row_info.is_hidden();
    for (size_t i = row_info.get_position(),
            n = row_info.get_col_row_repeated() + row_info.get_position(); i < n; ++i)
    {
        p_sheet_props->set_row_height(i, row_size, length_unit_t::point);
        p_sheet_props->set_row_hidden(i, hidden);
    }
}

void gnumeric_sheet_context::start_style(const xml_attrs_t& attrs)
{
    const gnumeric_style_attr_parser& attr_parser = for_each(attrs.begin(), attrs.end(), gnumeric_style_attr_parser(*mp_factory->get_styles(), front_color));
    spreadsheet::iface::import_styles& styles = *mp_factory->get_styles();
    if (attr_parser.is_fill_set())
    {
        size_t fill_id = styles.commit_fill();
        styles.set_xf_fill(fill_id);
    }
    if (attr_parser.is_protection_set())
    {
        size_t protection_id = styles.commit_cell_protection();
        styles.set_xf_protection(protection_id);
    }
}

void gnumeric_sheet_context::start_style_region(const xml_attrs_t& attrs)
{
    mp_region_data.reset(new gnumeric_style_region());
    for_each(attrs.begin(), attrs.end(), gnumeric_style_region_attr_parser(*mp_region_data));
}

void gnumeric_sheet_context::start_condition(const xml_attrs_t& attrs)
{
    spreadsheet::iface::import_conditional_format* cond_format =
        mp_sheet->get_conditional_format();
    if (cond_format)
    {
        for_each(attrs.begin(), attrs.end(), gnumeric_condition_attr_parser(cond_format));
    }
}

void gnumeric_sheet_context::end_table()
{
    mp_sheet = mp_factory->append_sheet(chars.get(), chars.size());
}

void gnumeric_sheet_context::end_font()
{
    spreadsheet::iface::import_styles& styles = *mp_factory->get_styles();
    styles.set_font_color(0, front_color.red, front_color.green, front_color.blue);
    styles.set_font_name(chars.get(), chars.size());
    size_t font_id = styles.commit_font();
    styles.set_xf_font(font_id);
}

void gnumeric_sheet_context::end_style(bool conditional_format)
{
    spreadsheet::iface::import_styles& styles = *mp_factory->get_styles();
    size_t id = styles.commit_cell_xf();
    if (!conditional_format)
    {
        mp_region_data->xf_id = id;
    }
    else
    {
        spreadsheet::iface::import_conditional_format* cond_format =
            mp_sheet->get_conditional_format();
        if (cond_format)
        {
            cond_format->set_xf_id(id);
        }
    }
}

void gnumeric_sheet_context::end_style_region()
{
    mp_sheet->set_format(mp_region_data->start_row, mp_region_data->start_col,
            mp_region_data->end_row, mp_region_data->end_col, mp_region_data->xf_id);

    if (mp_region_data->contains_conditional_format)
    {
        spreadsheet::iface::import_conditional_format* cond_format =
            mp_sheet->get_conditional_format();
        if (cond_format)
        {
            cond_format->set_range(mp_region_data->start_row, mp_region_data->start_col,
                    mp_region_data->end_row, mp_region_data->end_col);
            cond_format->commit_format();
        }
    }
    mp_region_data.reset();
}

void gnumeric_sheet_context::end_condition()
{
    spreadsheet::iface::import_conditional_format* cond_format =
        mp_sheet->get_conditional_format();
    if (cond_format)
    {
        cond_format->commit_entry();
    }
}

void gnumeric_sheet_context::end_expression()
{
    spreadsheet::iface::import_conditional_format* cond_format =
        mp_sheet->get_conditional_format();
    if (cond_format)
    {
        cond_format->set_formula(chars.get(), chars.size());
        cond_format->commit_condition();
    }
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
