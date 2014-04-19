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
                        m_styles.set_font_underline(spreadsheet::underline_none);
                    break;
                    case 1:
                        m_styles.set_font_underline(spreadsheet::underline_single);
                    break;
                    case 2:
                        m_styles.set_font_underline(spreadsheet::underline_double);
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
    gnumeric_style_attr_parser(spreadsheet::iface::import_styles& styles) :
        m_styles(styles),
        m_fill(false),
        m_protection(false) {}

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
            case XML_HAlign:
            {
                orcus::spreadsheet::hor_alignment_t hor_alignment = orcus::spreadsheet::hor_alignment_unknown;
                if (attr.value == "GNM_HALIGN_CENTER")
                    hor_alignment = orcus::spreadsheet::hor_alignment_center;
                else if (attr.value == "GNM_HALIGN_RIGHT")
                    hor_alignment = orcus::spreadsheet::hor_alignment_right;
                else if (attr.value == "GNM_HALIGN_LEFT")
                    hor_alignment = orcus::spreadsheet::hor_alignment_left;
                else if (attr.value == "GNM_HALIGN_JUSTIFY")
                    hor_alignment = orcus::spreadsheet::hor_alignment_justified;
                else if (attr.value == "GNM_HALIGN_DISTRIBUTED")
                    hor_alignment = orcus::spreadsheet::hor_alignment_distributed;
                else if (attr.value == "GNM_HALIGN_FILL")
                    hor_alignment = orcus::spreadsheet::hor_alignment_filled;

                if (hor_alignment != orcus::spreadsheet::hor_alignment_unknown)
                    m_styles.set_xf_apply_alignment(true);
                m_styles.set_xf_horizontal_alignment(hor_alignment);
            }
            break;
            case XML_VAlign:
            {
                orcus::spreadsheet::ver_alignment_t ver_alignment = orcus::spreadsheet::ver_alignment_unknown;
                if (attr.value == "GNM_VALIGN_BOTTOM")
                    ver_alignment = orcus::spreadsheet::ver_alignment_bottom;
                else if (attr.value == "GNM_VALIGN_TOP")
                    ver_alignment = orcus::spreadsheet::ver_alignment_top;
                else if (attr.value == "GNM_VALIGN_CENTER")
                    ver_alignment = orcus::spreadsheet::ver_alignment_middle;
                else if (attr.value == "GNM_VALIGN_JUSTIFY")
                    ver_alignment = orcus::spreadsheet::ver_alignment_justified;
                else if (attr.value == "GNM_VALIGN_DISTRIBUTED")
                    ver_alignment = orcus::spreadsheet::ver_alignment_distributed;

                if (ver_alignment != orcus::spreadsheet::ver_alignment_unknown)
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

class gnumeric_autofilter_field_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    gnumeric_autofilter_field_attr_parser(spreadsheet::iface::import_auto_filter& auto_filter):
        m_auto_filter(auto_filter) {}

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
            default:
                ;
        }
    }

private:
    spreadsheet::iface::import_auto_filter& m_auto_filter;
};

}


gnumeric_sheet_context::gnumeric_sheet_context(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory),
    mp_sheet(0)
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
        return mp_child.get();
    }

    return NULL;
}

void gnumeric_sheet_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void gnumeric_sheet_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    push_stack(ns, name);
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
                std::for_each(attrs.begin(), attrs.end(),
                        gnumeric_autofilter_attr_parser(
                            *mp_sheet->get_auto_filter()));
            }
            break;
            case XML_Field:
            {
                xml_token_pair_t parent = get_parent_element();
                assert(parent.first == NS_gnumeric_gnm && parent.second == XML_Filter);
                std::for_each(attrs.begin(), attrs.end(),
                        gnumeric_autofilter_field_attr_parser(
                            *mp_sheet->get_auto_filter()));
            }
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
                end_style();
            break;
            case XML_Filter:
                mp_sheet->get_auto_filter()->commit();
            break;
            case XML_Field:
                mp_sheet->get_auto_filter()->commit_column();
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
        p_sheet_props->set_column_width(i, col_size, length_unit_point);
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
        p_sheet_props->set_row_height(i, row_size, length_unit_point);
        p_sheet_props->set_row_hidden(i, hidden);
    }
}

void gnumeric_sheet_context::start_style(const xml_attrs_t& attrs)
{
    const gnumeric_style_attr_parser& attr_parser = for_each(attrs.begin(), attrs.end(), gnumeric_style_attr_parser(*mp_factory->get_styles()));
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

void gnumeric_sheet_context::end_table()
{
    mp_sheet = mp_factory->append_sheet(chars.get(), chars.size());
}

void gnumeric_sheet_context::end_font()
{
    spreadsheet::iface::import_styles& styles = *mp_factory->get_styles();
    styles.set_font_name(chars.get(), chars.size());
    size_t font_id = styles.commit_font();
    styles.set_xf_font(font_id);
}

void gnumeric_sheet_context::end_style()
{
    spreadsheet::iface::import_styles& styles = *mp_factory->get_styles();
    size_t id = styles.commit_cell_xf();
    mp_region_data->xf_id = id;
}

void gnumeric_sheet_context::end_style_region()
{
    for (spreadsheet::col_t col = mp_region_data->start_col;
            col <= mp_region_data->end_col; ++col)
    {
        for (spreadsheet::row_t row = mp_region_data->start_row;
                row <= mp_region_data->end_row; ++row)
        {
            mp_sheet->set_format(row, col, mp_region_data->xf_id);
        }
    }
    mp_region_data.reset();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
