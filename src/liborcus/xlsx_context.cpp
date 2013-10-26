/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_context.hpp"
#include "ooxml_global.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_types.hpp"
#include "ooxml_schemas.hpp"
#include "xml_context_global.hpp"

#include "orcus/global.hpp"
#include "orcus/tokens.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/measurement.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

bool to_rgb(
    const pstring& ps, spreadsheet::color_elem_t& alpha,
    spreadsheet::color_elem_t& red, spreadsheet::color_elem_t& green, spreadsheet::color_elem_t& blue)
{
    // RGB string is a 8-character string representing 32-bit hexadecimal
    // number e.g. 'FF004A12' (alpha - red - green - blue)
    size_t n = ps.size();
    if (n != 8)
        return false;

    unsigned long v = strtoul(ps.get(), NULL, 16);
    blue  = (0x000000FF & v);
    green = (0x000000FF & (v >> 8));
    red   = (0x000000FF & (v >> 16));
    alpha = (0x000000FF & (v >> 24));

    return true;
}

class shared_strings_root_attr_parser
{
public:
    shared_strings_root_attr_parser() : m_count(0), m_unique_count(0) {}

    void operator() (const xml_token_attr_t &attr)
    {
        switch (attr.name)
        {
            case XML_count:
                m_count = to_long(attr.value);
            break;
            case XML_uniqueCount:
                m_unique_count = to_long(attr.value);
            break;
        }
    }

    shared_strings_root_attr_parser& operator= (const shared_strings_root_attr_parser& r)
    {
        m_count = r.m_count;
        m_unique_count = r.m_unique_count;
        return *this;
    }

    size_t get_count() const { return m_count; }
    size_t get_unique_count() const { return m_unique_count; }
private:
    size_t m_count;
    size_t m_unique_count;
};

class color_attr_parser : unary_function<xml_token_attr_t, void>
{
    pstring m_rgb;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_rgb:
                m_rgb = attr.value;
            break;
            case XML_theme:
                // TODO : handle this.
            break;
            default:
                ;
        }
    }

    pstring get_rgb() const { return m_rgb; }
};

}

xlsx_shared_strings_context::xlsx_shared_strings_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_shared_strings* strings) :
    xml_context_base(session_cxt, tokens), mp_strings(strings), m_in_segments(false) {}

xlsx_shared_strings_context::~xlsx_shared_strings_context() {}

bool xlsx_shared_strings_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_shared_strings_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_shared_strings_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_shared_strings_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_sst:
        {
            // root element for the shared string part.
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);

            shared_strings_root_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);

            cout << "count: " << func.get_count() << "  unique count: " << func.get_unique_count() << endl;
        }
        break;
        case XML_si:
            // single shared string entry.
            m_in_segments = false;
            xml_element_expected(parent, NS_ooxml_xlsx, XML_sst);
        break;
        case XML_r:
            // rich text run
            m_in_segments = true;
            xml_element_expected(parent, NS_ooxml_xlsx, XML_si);
        break;
        case XML_rPr:
            // rich text run property
            xml_element_expected(parent, NS_ooxml_xlsx, XML_r);
        break;
        case XML_b:
            // bold
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
        break;
        case XML_i:
            // italic
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
        break;
        case XML_sz:
        {
            // font size
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
            pstring s = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            double point = to_double(s);
            mp_strings->set_segment_font_size(point);
        }
        break;
        case XML_color:
        {
            // font color
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
            color_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);

            spreadsheet::color_elem_t alpha;
            spreadsheet::color_elem_t red;
            spreadsheet::color_elem_t green;
            spreadsheet::color_elem_t blue;
            if (to_rgb(func.get_rgb(), alpha, red, green, blue))
                mp_strings->set_segment_font_color(alpha, red, green, blue);
        }
        break;
        case XML_rFont:
        {
            // font
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
            pstring font = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            mp_strings->set_segment_font_name(font.get(), font.size());
        }
        break;
        case XML_family:
            // font family
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
        break;
        case XML_scheme:
            // font scheme
            xml_element_expected(parent, NS_ooxml_xlsx, XML_rPr);
        break;
        case XML_t:
        {
            // actual text stored as its content.
            xml_elem_stack_t allowed;
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_si));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_r));
            xml_element_expected(parent, allowed);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_shared_strings_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_t:
        break;
        case XML_b:
            mp_strings->set_segment_bold(true);
        break;
        case XML_i:
            mp_strings->set_segment_italic(true);
        break;
        case XML_r:
            mp_strings->append_segment(m_cur_str.get(), m_cur_str.size());
        break;
        case XML_si:
        {
            if (m_in_segments)
                // commit all formatted segments.
                mp_strings->commit_segments();
            else
            {
                // unformatted text should only have one text segment.
                mp_strings->append(m_cur_str.get(), m_cur_str.size());
            }
        }
        break;
    }
    return pop_stack(ns, name);
}

void xlsx_shared_strings_context::characters(const pstring& str, bool transient)
{
    xml_token_pair_t& cur_token = get_current_element();
    if (cur_token.first == NS_ooxml_xlsx && cur_token.second == XML_t)
    {
        m_cur_str = str;
        if (transient)
            m_cur_str = m_pool.intern(m_cur_str).first;
    }
}

// ============================================================================

namespace {

class border_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::border_direction_t m_dir;
    spreadsheet::iface::import_styles& m_styles;
public:
    border_attr_parser(spreadsheet::border_direction_t dir, spreadsheet::iface::import_styles& styles) :
        m_dir(dir), m_styles(styles) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_style:
                m_styles.set_border_style(m_dir, attr.value.get(), attr.value.size());
            break;
        }
    }
};

class cell_style_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::iface::import_styles& m_styles;
public:
    cell_style_attr_parser(spreadsheet::iface::import_styles& styles) :
        m_styles(styles) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_name:
                m_styles.set_cell_style_name(attr.value.get(), attr.value.size());
            break;
            case XML_xfId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_cell_style_xf(n);
            }
            break;
            case XML_builtinId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_cell_style_builtin(n);
            }
            break;
        }
    }
};

class xf_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::iface::import_styles& m_styles;
public:
    xf_attr_parser(spreadsheet::iface::import_styles& styles) :
        m_styles(styles) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_borderId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_xf_border(n);
            }
            break;
            case XML_fillId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_xf_fill(n);
            }
            break;
            case XML_fontId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_xf_font(n);
            }
            break;
            case XML_numFmtId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_xf_number_format(n);
            }
            break;
            case XML_xfId:
            {
                size_t n = to_long(attr.value);
                m_styles.set_xf_style_xf(n);
            }
            break;
            case XML_applyBorder:
            break;
            case XML_applyFill:
            break;
            case XML_applyFont:
            break;
            case XML_applyNumberFormat:
            break;
            case XML_applyAlignment:
            {
                bool b = to_long(attr.value) != 0;
                m_styles.set_xf_apply_alignment(b);
            }
            break;
        }
    }
};

class fill_color_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::iface::import_styles& m_styles;
    const tokens& m_tokens;
    bool m_foreground;
public:
    fill_color_attr_parser(spreadsheet::iface::import_styles& styles, const tokens& _tokens, bool fg) :
        m_styles(styles), m_tokens(_tokens), m_foreground(fg) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_rgb:
            {
                spreadsheet::color_elem_t alpha;
                spreadsheet::color_elem_t red;
                spreadsheet::color_elem_t green;
                spreadsheet::color_elem_t blue;
                if (!to_rgb(attr.value, alpha, red, green, blue))
                    // invalid RGB color format.
                    return;

                if (m_foreground)
                    m_styles.set_fill_fg_color(alpha, red, green, blue);
                else
                    m_styles.set_fill_bg_color(alpha, red, green, blue);
            }
            break;
            case XML_indexed:
            break;
            default:
                cerr << "warning: unknown attribute [ " << m_tokens.get_token_name(attr.name) << " ]" << endl;
        }
    }
};

class cell_protection_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::iface::import_styles& m_styles;
public:

    cell_protection_attr_parser(spreadsheet::iface::import_styles& styles) :
        m_styles(styles) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_hidden:
            {
                bool b = to_long(attr.value) != 0;
                m_styles.set_cell_hidden(b);
            }
            break;
            case XML_locked:
            {
                bool b = to_long(attr.value) != 0;
                m_styles.set_cell_locked(b);
            }
            break;
        }
    }
};

class cell_alignment_attr_parser : public unary_function<xml_token_attr_t, void>
{
    spreadsheet::hor_alignment_t m_hor_align;

public:
    cell_alignment_attr_parser() : m_hor_align(spreadsheet::hor_alignment_unknown) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_horizontal:
            {
                if (attr.value == "center")
                    m_hor_align = spreadsheet::hor_alignment_center;
                else if (attr.value == "right")
                    m_hor_align = spreadsheet::hor_alignment_right;
                else if (attr.value == "left")
                    m_hor_align = spreadsheet::hor_alignment_left;
            }
            break;
            default:
                ;
        }
    }

    spreadsheet::hor_alignment_t get_hor_align() const
    {
        return m_hor_align;
    }
};

}

xlsx_styles_context::xlsx_styles_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_styles* styles) :
    xml_context_base(session_cxt, tokens),
    mp_styles(styles),
    m_cur_border_dir(spreadsheet::border_direction_unknown),
    m_cell_style_xf(false) {}

xlsx_styles_context::~xlsx_styles_context() {}

bool xlsx_styles_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_styles_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_styles_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_styles_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_styleSheet:
        {
            // root element
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);
        }
        break;
        case XML_fonts:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t font_count = to_long(ps);
            mp_styles->set_font_count(font_count);
        }
        break;
        case XML_font:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_fonts);
        break;
        case XML_b:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
            mp_styles->set_font_bold(true);
        break;
        case XML_i:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
            mp_styles->set_font_italic(true);
        break;
        case XML_u:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            if (ps == "double")
                mp_styles->set_font_underline(spreadsheet::underline_double);
            else if (ps == "single")
                mp_styles->set_font_underline(spreadsheet::underline_single);
            else if (ps == "singleAccounting")
                mp_styles->set_font_underline(spreadsheet::underline_single_accounting);
            else if (ps == "doubleAccounting")
                mp_styles->set_font_underline(spreadsheet::underline_double_accounting);
        }
        break;
        case XML_sz:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            double font_size = to_double(ps);
            mp_styles->set_font_size(font_size);
        }
        break;
        case XML_color:
        {
            // The color element can occur under various parent elements.
            xml_elem_stack_t allowed;
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_font));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_top));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_bottom));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_left));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_right));
            allowed.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_mruColors));
            xml_element_expected(parent, allowed);

            if (parent.first == NS_ooxml_xlsx)
            {
                switch (parent.second)
                {
                    case XML_top:
                    case XML_bottom:
                    case XML_left:
                    case XML_right:
                        // This color is for a border.
                        start_border_color(attrs);
                    break;
                    case XML_font:
                        start_font_color(attrs);
                    default:
                        ;
                }
            }
        }
        break;
        case XML_name:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_val)).get_value();
            mp_styles->set_font_name(ps.get(), ps.size());
        }
        break;
        case XML_family:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
        break;
        case XML_scheme:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_font);
        break;
        case XML_fills:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t fill_count = to_long(ps);
            mp_styles->set_fill_count(fill_count);
        }
        break;
        case XML_fill:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_fills);
        break;
        case XML_patternFill:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_fill);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_patternType)).get_value();
            mp_styles->set_fill_pattern_type(ps.get(), ps.size());
        }
        break;
        case XML_fgColor:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_patternFill);
            for_each(attrs.begin(), attrs.end(), fill_color_attr_parser(*mp_styles, get_tokens(), true));
        }
        break;
        case XML_bgColor:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_patternFill);
            for_each(attrs.begin(), attrs.end(), fill_color_attr_parser(*mp_styles, get_tokens(), false));
        }
        break;
        case XML_borders:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t border_count = to_long(ps);
            mp_styles->set_border_count(border_count);
        }
        break;
        case XML_border:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_borders);
        break;
        case XML_top:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_border);
            m_cur_border_dir = spreadsheet::border_top;
            border_attr_parser func(spreadsheet::border_top, *mp_styles);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_bottom:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_border);
            m_cur_border_dir = spreadsheet::border_bottom;
            border_attr_parser func(spreadsheet::border_bottom, *mp_styles);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_left:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_border);
            m_cur_border_dir = spreadsheet::border_left;
            border_attr_parser func(spreadsheet::border_left, *mp_styles);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_right:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_border);
            m_cur_border_dir = spreadsheet::border_right;
            border_attr_parser func(spreadsheet::border_right, *mp_styles);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_diagonal:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_border);
            m_cur_border_dir = spreadsheet::border_diagonal;
            border_attr_parser func(spreadsheet::border_diagonal, *mp_styles);
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_cellStyleXfs:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(
                attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t n = strtoul(ps.get(), NULL, 10);
            mp_styles->set_cell_style_xf_count(n);
            m_cell_style_xf = true;
        }
        break;
        case XML_cellXfs:
        {
            // Collection of un-named cell formats used in the document.
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(
                attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t n = strtoul(ps.get(), NULL, 10);
            mp_styles->set_cell_xf_count(n);
            m_cell_style_xf = false;
        }
        break;
        case XML_cellStyles:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_styleSheet);
            pstring ps = for_each(
                attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_count)).get_value();
            size_t n = strtoul(ps.get(), NULL, 10);
            mp_styles->set_cell_style_count(n);
        }
        break;
        case XML_cellStyle:
        {
            // named cell style, some of which are built-in such as 'Normal'.
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cellStyles);
            for_each(attrs.begin(), attrs.end(), cell_style_attr_parser(*mp_styles));
        }
        break;
        case XML_xf:
        {
            // Actual cell format attributes (for some reason) abbreviated to
            // 'xf'.  Used both by cells and cell styles.
            xml_elem_stack_t allowed;
            allowed.push_back(xml_elem_stack_t::value_type(NS_ooxml_xlsx, XML_cellXfs));
            allowed.push_back(xml_elem_stack_t::value_type(NS_ooxml_xlsx, XML_cellStyleXfs));
            xml_element_expected(parent, allowed);

            for_each(attrs.begin(), attrs.end(), xf_attr_parser(*mp_styles));
        }
        break;
        case XML_protection:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_xf);
            for_each(attrs.begin(), attrs.end(), cell_protection_attr_parser(*mp_styles));
        }
        break;
        case XML_alignment:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_xf);
            cell_alignment_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            mp_styles->set_xf_horizontal_alignment(func.get_hor_align());
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_styles_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_font:
            mp_styles->commit_font();
        break;
        case XML_fill:
            mp_styles->commit_fill();
        break;
        case XML_border:
            mp_styles->commit_border();
        break;
        case XML_cellStyle:
            mp_styles->commit_cell_style();
        break;
        case XML_xf:
            if (m_cell_style_xf)
                mp_styles->commit_cell_style_xf();
            else
                mp_styles->commit_cell_xf();
        break;
        case XML_protection:
        {
            size_t id = mp_styles->commit_cell_protection();
            mp_styles->set_xf_protection(id);
        }
        break;
    }
    return pop_stack(ns, name);
}

void xlsx_styles_context::characters(const pstring& /*str*/, bool /*transient*/)
{
    // not used in the styles.xml part.
}

void xlsx_styles_context::start_border_color(const xml_attrs_t& attrs)
{
    pstring ps = for_each(
        attrs.begin(), attrs.end(), single_attr_getter(m_pool, NS_ooxml_xlsx, XML_rgb)).get_value();

    spreadsheet::color_elem_t alpha;
    spreadsheet::color_elem_t red;
    spreadsheet::color_elem_t green;
    spreadsheet::color_elem_t blue;
    if (to_rgb(ps, alpha, red, green, blue))
        mp_styles->set_border_color(m_cur_border_dir, alpha, red, green, blue);
}

void xlsx_styles_context::start_font_color(const xml_attrs_t& attrs)
{
    color_attr_parser func;
    func = for_each(attrs.begin(), attrs.end(), func);

    spreadsheet::color_elem_t alpha;
    spreadsheet::color_elem_t red;
    spreadsheet::color_elem_t green;
    spreadsheet::color_elem_t blue;
    if (to_rgb(func.get_rgb(), alpha, red, green, blue))
        mp_styles->set_font_color(alpha, red, green, blue);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
