/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_styles_context.hpp"
#include "odf_namespace_types.hpp"
#include "odf_token_constants.hpp"
#include "odf_helper.hpp"
#include "orcus/measurement.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "odf_number_formatting_context.hpp"
#include <iostream>

using namespace std;

namespace orcus {

namespace {

class style_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    const style_value_converter* m_converter;

    pstring m_name;
    odf_style_family m_family;

    pstring m_parent_name;
public:
    style_attr_parser(const style_value_converter* converter) :
        m_converter(converter), m_family(style_family_unknown) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_name = attr.value;
                break;
                case XML_family:
                    m_family = m_converter->to_style_family(attr.value);
                break;
                case XML_parent_style_name:
                    m_parent_name = attr.value;
            }
        }
    }

    const pstring& get_name() const { return m_name; }
    odf_style_family get_family() const { return m_family; }
    const pstring& get_parent() const { return m_parent_name; }
};

class col_prop_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    length_t m_width;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_column_width:
                    m_width = to_length(attr.value);
                break;
            }
        }
    }

    const length_t& get_width() const { return m_width; }
};

class row_prop_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    length_t m_height;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_row_height:
                    m_height = to_length(attr.value);
                break;
            }
        }
    }

    const length_t& get_height() const { return m_height; }
};

class text_prop_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_font_name;
    length_t m_font_size;
    bool m_bold;
    bool m_italic;
    bool m_color;

    spreadsheet::color_elem_t m_red;
    spreadsheet::color_elem_t m_green;
    spreadsheet::color_elem_t m_blue;

    bool m_underline_is_text_color;
    bool m_underline;

    spreadsheet::color_elem_t m_underline_red;
    spreadsheet::color_elem_t m_underline_green;
    spreadsheet::color_elem_t m_underline_blue;

    spreadsheet::underline_mode_t m_underline_mode;
    spreadsheet::underline_width_t m_underline_width;
    spreadsheet::underline_t m_underline_style;
    spreadsheet::underline_type_t m_underline_type;

public:
    text_prop_attr_parser() : m_bold(false), m_italic(false), m_color(false),
                            m_underline_is_text_color(false), m_underline(false),
                            m_underline_mode(spreadsheet::underline_mode_t::continuos),
                            m_underline_width(spreadsheet::underline_width_t::none),
                            m_underline_style(spreadsheet::underline_t::none),
                            m_underline_type(spreadsheet::underline_type_t::none) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_font_name:
                    m_font_name = attr.value;
                break;
                case XML_text_underline_color:
                    if (!odf_helper::convert_fo_color(attr.value, m_underline_red, m_underline_green, m_underline_blue))
                    {
                        m_underline = true;
                        m_underline_is_text_color = true;
                    }
                break;
                case XML_text_underline_mode:
                    m_underline = true;
                    if (attr.value == "skip-white-space")
                        m_underline_mode = spreadsheet::underline_mode_t::skip_white_space;
                    else
                        m_underline_mode = spreadsheet::underline_mode_t::continuos;
                break;
                case XML_text_underline_width:
                {
                    m_underline = true;
                    m_underline_width = odf_helper::extract_underline_width(attr.value);
                }
                break;
                case XML_text_underline_style:
                {
                    m_underline = true;
                    m_underline_style = odf_helper::extract_underline_style(attr.value);
                }
                break;
                case XML_text_underline_type:
                {
                    m_underline = true;
                    if (attr.value == "none")
                        m_underline_type = spreadsheet::underline_type_t::none;
                    if (attr.value == "single")
                        m_underline_type = spreadsheet::underline_type_t::single;
                    if (attr.value == "double")
                        m_underline_type = spreadsheet::underline_type_t::double_type;
                }
                break;
                default:
                    ;
            }
        }
        else if (attr.ns == NS_odf_fo)
        {
            switch (attr.name)
            {
                case XML_font_size:
                    m_font_size = to_length(attr.value);
                break;
                case XML_font_style:
                    m_italic = attr.value == "italic";
                break;
                case XML_font_weight:
                    m_bold = attr.value == "bold";
                break;
                case XML_color:
                    m_color = odf_helper::convert_fo_color(attr.value,
                            m_red, m_green, m_blue);
                break;
                default:
                    ;
            }
        }
    }

    pstring get_font_name() const { return m_font_name; }
    length_t get_font_size() const { return m_font_size; }
    bool is_bold() const { return m_bold; }
    bool is_italic() const { return m_italic; }
    bool has_color() const { return m_color; }
    void get_color(spreadsheet::color_elem_t& red, spreadsheet::color_elem_t& green,
            spreadsheet::color_elem_t& blue)
    {
        red = m_red;
        green = m_green;
        blue = m_blue;
    }
    bool has_underline() const { return m_underline; }
    bool underline_is_text_color() const { return m_underline_is_text_color; }
    const spreadsheet::underline_width_t get_underline_width() const { return m_underline_width; }
    const spreadsheet::underline_mode_t get_underline_mode() const { return m_underline_mode; }
    const spreadsheet::underline_type_t get_underline_type() const { return m_underline_type; }
    const spreadsheet::underline_t get_underline_style() const { return m_underline_style; }
    void get_underline_color(spreadsheet::color_elem_t& red, spreadsheet::color_elem_t& green,
            spreadsheet::color_elem_t& blue)
    {
        red = m_underline_red;
        green = m_underline_green;
        blue = m_underline_blue;
    }
};

class cell_prop_attr_parser : std::unary_function<xml_token_attr_t, void>
{
public:
    typedef std::map<spreadsheet::border_direction_t, odf_helper::odf_border_details> border_map_type;

    cell_prop_attr_parser():
        m_background_red(0),
        m_background_green(0),
        m_background_blue(0),
        m_background_color(false),
        m_locked(false),
        m_hidden(false),
        m_formula_hidden(false),
        m_print_content(false),
        m_cell_protection(false)
    {}

private:

    spreadsheet::color_elem_t m_background_red;
    spreadsheet::color_elem_t m_background_green;
    spreadsheet::color_elem_t m_background_blue;

    bool m_background_color;
    bool m_locked;
    bool m_hidden;
    bool m_formula_hidden;
    bool m_print_content;
    bool m_cell_protection;

    border_map_type m_border_style_dir_pair;

public:

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_fo)
        {
            switch (attr.name)
            {
                case XML_background_color:
                    m_background_color = odf_helper::convert_fo_color(attr.value, m_background_red,
                            m_background_green, m_background_blue);
                break;

                case XML_border:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::top, border_details));
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::bottom, border_details));
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::left, border_details));
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::right, border_details));
                }
                break;

                case XML_border_top:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::top, border_details));
                }
                break;

                case XML_border_bottom:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::bottom, border_details));
                }
                break;

                case XML_border_left:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::left, border_details));
                }
                break;

                case XML_border_right:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::right, border_details));
                }
                break;
                case XML_diagonal_bl_tr:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::diagonal_bl_tr, border_details));
                }
                break;
                case XML_diagonal_tl_br:
                {
                    odf_helper::odf_border_details border_details = odf_helper::extract_border_details(attr.value);
                    m_border_style_dir_pair.insert(std::make_pair(spreadsheet::border_direction_t::diagonal_tl_br, border_details));
                }
                break;

                default:
                    ;
            }
        }

        else if(attr.ns == NS_odf_style)
        {
            switch(attr.name)
            {
                case XML_print_content:
                {
                    m_cell_protection = true;
                    m_print_content = attr.value == "true";
                }
                break;
                case XML_cell_protect:
                {
                    m_cell_protection = true;
                    if (attr.value == "protected")
                        m_locked = true;
                    else if (attr.value == "hidden-and-protected")
                    {
                        m_locked = true;
                        m_hidden = true;
                    }
                    else if (attr.value == "formula-hidden")
                        m_formula_hidden = true;
                    else if (attr.value == "protected formula-hidden" || attr.value == "formula-hidden protected")
                    {
                        m_formula_hidden = true;
                        m_locked = true;
                    }
                }
                default:
                    ;
            }
        }
    }

    bool has_background_color() const { return m_background_color; }

    void get_background_color(spreadsheet::color_elem_t& red,
            spreadsheet::color_elem_t& green, spreadsheet::color_elem_t& blue)
    {
        red = m_background_red;
        green = m_background_green;
        blue = m_background_blue;
    }

    bool has_border() const { return !m_border_style_dir_pair.empty(); }

    bool has_protection() const { return m_cell_protection; }
    bool is_locked() const { return m_locked; }
    bool is_hidden() const { return m_hidden; }
    bool is_formula_hidden() const { return m_formula_hidden; }
    bool is_print_content() const { return m_print_content; }

    const border_map_type& get_border_attrib() const
    {
        return m_border_style_dir_pair;
    }

};

}

style_value_converter::style_value_converter()
{
    static const struct {
        const char* str;
        odf_style_family val;
    } style_family_values[] = {
        { "graphic", style_family_graphic },
        { "paragraph", style_family_paragraph },
        { "table", style_family_table },
        { "table-column", style_family_table_column },
        { "table-row", style_family_table_row },
        { "table-cell", style_family_table_cell },
        { "text", style_family_text }
    };

    size_t n = sizeof(style_family_values)/sizeof(style_family_values[0]);
    for (size_t i = 0; i < n; ++i)
    {
        m_style_families.insert(
            style_families_type::value_type(
                style_family_values[i].str, style_family_values[i].val));
    }
}

odf_style_family style_value_converter::to_style_family(const pstring& val) const
{
    style_families_type::const_iterator it = m_style_families.find(val);
    return it == m_style_families.end() ? style_family_unknown : it->second;
}

styles_context::styles_context(
    session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles,
    spreadsheet::iface::import_styles* iface_styles) :
    xml_context_base(session_cxt, tk),
    mp_styles(iface_styles),
    m_styles(styles),
    m_automatic_styles(false)
{
    commit_default_styles();
}

bool styles_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_odf_number)
        return false;

    return true;
}

xml_context_base* styles_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    number_formatting_style* number_formatting = new number_formatting_style;
    if (ns == NS_odf_number )
    {
        mp_child.reset(new number_formatting_context(get_session_context(), get_tokens(), m_styles, mp_styles, number_formatting));
        mp_child->transfer_common(*this);
        return mp_child.get();
    }

    return nullptr;
}

void styles_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
    return;
}

void styles_context::start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_odf_office)
    {
        switch (name)
        {
            case XML_automatic_styles:
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
                m_automatic_styles = true;
            break;
            case XML_styles:
                m_automatic_styles = false;
            break;
            default:
                warn_unhandled();
        }
    }
    else if (ns == NS_odf_style)
    {
        switch (name)
        {
            case XML_style:
            {
                xml_elem_stack_t expected_parents;
                expected_parents.push_back(std::pair<xmlns_id_t, xml_token_t>(NS_odf_office, XML_automatic_styles));
                expected_parents.push_back(std::pair<xmlns_id_t, xml_token_t>(NS_odf_office, XML_styles));
                xml_element_expected(parent, expected_parents);
                style_attr_parser func(&m_converter);
                func = std::for_each(attrs.begin(), attrs.end(), func);
                m_current_style.reset(new odf_style(func.get_name(), func.get_family(), func.get_parent()));
            }
            break;
            case XML_table_column_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                col_prop_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                assert(m_current_style->family == style_family_table_column);
                m_current_style->column_data->width = func.get_width();
            }
            break;
            case XML_table_row_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                row_prop_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                assert(m_current_style->family == style_family_table_row);
                m_current_style->row_data->height = func.get_height();
            }
            break;
            case XML_table_properties:
                xml_element_expected(parent, NS_odf_style, XML_style);
            break;
            case XML_paragraph_properties:
                xml_element_expected(parent, NS_odf_style, XML_style);
            break;
            case XML_text_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                if (mp_styles)
                {
                    text_prop_attr_parser func;
                    func = std::for_each(attrs.begin(), attrs.end(), func);

                    // Commit the font data.
                    pstring font_name = func.get_font_name();
                    if (!font_name.empty())
                        mp_styles->set_font_name(font_name.get(), font_name.size());

                    length_t font_size = func.get_font_size();
                    if (font_size.unit == length_unit_t::point)
                        mp_styles->set_font_size(font_size.value);

                    if (func.is_bold())
                        mp_styles->set_font_bold(true);

                    if (func.is_italic())
                        mp_styles->set_font_italic(true);

                    if (func.has_color())
                    {
                        spreadsheet::color_elem_t red, green, blue;
                        func.get_color(red, green, blue);
                        mp_styles->set_font_color(0, red, green, blue);
                    }

                    if (func.has_underline())
                    {
                        if (func.underline_is_text_color() && func.has_color())
                        {
                            spreadsheet::color_elem_t red, green, blue;
                            func.get_color(red, green, blue);
                            mp_styles->set_font_underline_color(0, red, green, blue);
                        }
                        else
                        {
                            spreadsheet::color_elem_t red, green, blue;
                            func.get_underline_color(red, green, blue);
                            mp_styles->set_font_underline_color(0, red, green, blue);
                        }
                        spreadsheet::underline_width_t width = func.get_underline_width();
                        mp_styles->set_font_underline_width(width);

                        spreadsheet::underline_t style = func.get_underline_style();
                        mp_styles->set_font_underline(style);

                        spreadsheet::underline_type_t type = func.get_underline_type();
                        mp_styles->set_font_underline_type(type);

                        spreadsheet::underline_mode_t mode = func.get_underline_mode();
                        mp_styles->set_font_underline_mode(mode);
                    }

                    size_t font_id = mp_styles->commit_font();

                    switch (m_current_style->family)
                    {
                        case style_family_table_cell:
                        {
                            odf_style::cell* data = m_current_style->cell_data;
                            data->font = font_id;
                        }
                        break;
                        case style_family_text:
                        {
                            odf_style::text* data = m_current_style->text_data;
                            data->font = font_id;
                        }
                        break;
                        default:
                            ;
                    }
                }
            }
            break;
            case XML_table_cell_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                assert(m_current_style->family == style_family_table_cell);
                m_current_style->cell_data->automatic_style = m_automatic_styles;
                if (mp_styles)
                {
                    cell_prop_attr_parser func;
                    func = std::for_each(attrs.begin(), attrs.end(), func);
                    if (func.has_background_color())
                    {
                        spreadsheet::color_elem_t red, green, blue;
                        func.get_background_color(red, green, blue);
                        mp_styles->set_fill_bg_color(0, red, green, blue);
                    }

                    size_t fill_id = mp_styles->commit_fill();

                    if (func.has_border())
                    {
                        const cell_prop_attr_parser::border_map_type& border_map = func.get_border_attrib();
                        for (cell_prop_attr_parser::border_map_type::const_iterator itr = border_map.begin(); itr != border_map.end(); ++itr)
                        {
                            mp_styles->set_border_color(itr->first, 0, itr->second.red, itr->second.green, itr->second.blue);
                            mp_styles->set_border_style(itr->first, itr->second.border_style);
                            mp_styles->set_border_width(itr->first, itr->second.border_width);
                        }
                    }

                    size_t border_id = mp_styles->commit_border();

                    if (func.has_protection())
                    {
                        mp_styles->set_cell_hidden(func.is_hidden());
                        mp_styles->set_cell_locked(func.is_locked());
                        mp_styles->set_cell_print_content(func.is_print_content());
                        mp_styles->set_cell_formula_hidden(func.is_formula_hidden());
                    }
                    size_t cell_protection_id = mp_styles->commit_cell_protection();
                    switch (m_current_style->family)
                    {
                        case style_family_table_cell:
                        {
                            odf_style::cell* data = m_current_style->cell_data;
                            data->fill = fill_id;
                            data->border = border_id;
                            data->protection = cell_protection_id;
                        }
                        break;
                        default:
                            ;
                    }
                }
            }
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool styles_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_style)
    {
        switch (name)
        {
            case XML_style:
            {
                if (m_current_style)
                {
                    if (mp_styles && m_current_style->family == style_family_table_cell)
                    {
                        odf_style::cell& cell = *m_current_style->cell_data;
                        mp_styles->set_xf_font(cell.font);
                        mp_styles->set_xf_fill(cell.fill);
                        mp_styles->set_xf_border(cell.border);
                        mp_styles->set_xf_protection(cell.protection);
                        size_t xf_id = 0;
                        if (cell.automatic_style)
                            xf_id = mp_styles->commit_cell_xf();
                        else
                        {
                            size_t style_xf_id = mp_styles->commit_cell_style_xf();
                            mp_styles->set_cell_style_name(
                                    m_current_style->name.get(), m_current_style->name.size());
                            mp_styles->set_cell_style_xf(style_xf_id);
                            mp_styles->set_cell_style_parent_name(
                                    m_current_style->parent_name.get(), m_current_style->parent_name.size());

                            xf_id = mp_styles->commit_cell_style();
                        }
                        cell.xf = xf_id;
                    }

                    // ptr_map's first argument must be a non-const reference.
                    pstring name = m_current_style->name;
                    m_styles.insert(
                        odf_styles_map_type::value_type(
                            name, std::move(m_current_style)));
                    assert(!m_current_style);
                }
            }
            break;
        }
    }
    return pop_stack(ns, name);
}

void styles_context::characters(const pstring& str, bool transient)
{
}

void styles_context::commit_default_styles()
{
    if (!mp_styles)
        return;

    // Set default styles. Default styles must be associated with an index of 0.
    // Set empty styles for all style types before importing real styles.
    mp_styles->commit_font();
    mp_styles->commit_fill();
    mp_styles->commit_border();
    mp_styles->commit_cell_protection();
    mp_styles->commit_number_format();
    mp_styles->commit_cell_style();
    mp_styles->commit_cell_style_xf();
    mp_styles->commit_cell_xf();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
