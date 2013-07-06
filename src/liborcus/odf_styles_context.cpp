/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#include "odf_styles_context.hpp"
#include "odf_namespace_types.hpp"
#include "odf_token_constants.hpp"

#include "orcus/measurement.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

class style_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    const style_value_converter* m_converter;

    pstring m_name;
    odf_style_family m_family;
public:
    style_attr_parser(const style_value_converter* converter) : m_converter(converter), m_family(style_family_unknown) {}

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
            }
        }
    }

    const pstring& get_name() const { return m_name; }
    odf_style_family get_family() const { return m_family; }
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

public:
    text_prop_attr_parser() : m_bold(false), m_italic(false) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_font_name:
                    m_font_name = attr.value;
                break;
                case XML_font_size:
                    m_font_size = to_length(attr.value);
                break;
                default:
                    ;
            }
        }
        else if (attr.ns == NS_odf_fo)
        {
            switch (attr.name)
            {
                case XML_font_style:
                    m_italic = attr.value == "italic";
                break;
                case XML_font_weight:
                    m_bold = attr.value == "bold";
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

automatic_styles_context::automatic_styles_context(
    session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles,
    spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tk),
    mp_factory(factory),
    m_styles(styles)
{
    spreadsheet::iface::import_styles* p_styles = mp_factory->get_styles();
    if (p_styles)
    {
        // Set default styles. Default styles must be associated with an index of 0.
        // Set empty styles for all style types before importing real styles.
        p_styles->commit_font();
        p_styles->commit_fill();
        p_styles->commit_border();
        p_styles->commit_cell_protection();
        p_styles->commit_number_format();
        p_styles->commit_cell_style();
        p_styles->commit_cell_style_xf();
        p_styles->commit_cell_xf();
    }
}

bool automatic_styles_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* automatic_styles_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void automatic_styles_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void automatic_styles_context::start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_odf_office)
    {
        switch (name)
        {
            case XML_automatic_styles:
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
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
                xml_element_expected(parent, NS_odf_office, XML_automatic_styles);
                style_attr_parser func(&m_converter);
                func = std::for_each(attrs.begin(), attrs.end(), func);
                m_current_style.reset(new odf_style(func.get_name(), func.get_family()));
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
                spreadsheet::iface::import_styles* styles = mp_factory->get_styles();
                if (styles)
                {
                    text_prop_attr_parser func;
                    func = std::for_each(attrs.begin(), attrs.end(), func);

                    // Commit the font data.
                    pstring font_name = func.get_font_name();
                    if (!font_name.empty())
                        styles->set_font_name(font_name.get(), font_name.size());

                    length_t font_size = func.get_font_size();
                    if (font_size.unit == length_unit_point)
                        styles->set_font_size(font_size.value);

                    if (func.is_bold())
                        styles->set_font_bold(true);

                    if (func.is_italic())
                        styles->set_font_italic(true);

                    size_t font_id = styles->commit_font();

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
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool automatic_styles_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_style)
    {
        switch (name)
        {
            case XML_style:
            {
                if (m_current_style)
                {
                    // ptr_map's first argument must be a non-const reference.
                    pstring name = m_current_style->name;
                    m_styles.insert(name, m_current_style.release());
                }
            }
            break;
        }
    }
    return pop_stack(ns, name);
}

void automatic_styles_context::characters(const pstring& str, bool transient)
{
}

}
