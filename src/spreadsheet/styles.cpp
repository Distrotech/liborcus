/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/styles.hpp"
#include "orcus/string_pool.hpp"

#include <algorithm>
#include <cassert>

namespace orcus { namespace spreadsheet {

font_t::font_t() :
    size(0.0), bold(false),
    italic(false), underline_style(underline_t::none),
    underline_width(underline_width_t::none),
    underline_mode(underline_mode_t::continuos),
    underline_type(underline_type_t::none),
    color(),
    strikeout_style(strikeout_style_t::unknown),
    strikeout_width(strikeout_width_t::unknown),
    strikeout_type(strikeout_type_t::unknown),
    strikeout_text(strikeout_text_t::unknown)
{
}

void font_t::reset()
{
    *this = font_t();
}

color_t::color_t() :
    alpha(0), red(0), green(0), blue(0)
{
}

color_t::color_t(color_elem_t _alpha, color_elem_t _red, color_elem_t _green, color_elem_t _blue) :
    alpha(_alpha), red(_red), green(_green), blue(_blue)
{
}

void color_t::reset()
{
    *this = color_t();
}

fill_t::fill_t()
{
}

void fill_t::reset()
{
    *this = fill_t();
}

border_attrs_t::border_attrs_t():
    style(orcus::spreadsheet::border_style_t::unknown)
{
}

void border_attrs_t::reset()
{
    *this = border_attrs_t();
}

border_t::border_t()
{
}

void border_t::reset()
{
    *this = border_t();
}

protection_t::protection_t() :
    locked(false), hidden(false), print_content(false), formula_hidden(false)
{
}

void protection_t::reset()
{
    *this = protection_t();
}

number_format_t::number_format_t() : identifier(0) {}

void number_format_t::reset()
{
    *this = number_format_t();
}

bool number_format_t::operator== (const number_format_t& r) const
{
    return format_string == r.format_string;
}

cell_format_t::cell_format_t() :
    font(0),
    fill(0),
    border(0),
    protection(0),
    number_format(0),
    style_xf(0),
    hor_align(hor_alignment_t::unknown),
    ver_align(ver_alignment_t::unknown),
    apply_num_format(false),
    apply_font(false),
    apply_fill(false),
    apply_border(false),
    apply_alignment(false),
    apply_protection(false)
{
}

void cell_format_t::reset()
{
    *this = cell_format_t();
}

cell_style_t::cell_style_t() :
    xf(0), builtin(0)
{
}

void cell_style_t::reset()
{
    *this = cell_style_t();
}

import_styles::import_styles(string_pool& sp) : m_string_pool(sp) {}

import_styles::~import_styles()
{
}

void import_styles::set_font_count(size_t n)
{
    m_fonts.reserve(n);
}

void import_styles::set_font_bold(bool b)
{
    m_cur_font.bold = b;
}

void import_styles::set_font_italic(bool b)
{
    m_cur_font.italic = b;
}

void import_styles::set_font_name(const char* s, size_t n)
{
    m_cur_font.name = m_string_pool.intern(s, n).first;
}

void import_styles::set_font_size(double point)
{
    m_cur_font.size = point;
}

void import_styles::set_font_underline(underline_t e)
{
    m_cur_font.underline_style = e;
}

void import_styles::set_font_underline_width(underline_width_t e)
{
    m_cur_font.underline_width = e;
}

void import_styles::set_font_underline_mode(underline_mode_t e)
{
    m_cur_font.underline_mode = e;
}

void import_styles::set_font_underline_type(underline_type_t e)
{
    m_cur_font.underline_type = e;
}

void import_styles::set_font_underline_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_font.underline_color = color_t(alpha, red, green, blue);
}

void import_styles::set_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_font.color = color_t(alpha, red, green, blue);
}

void import_styles::set_strikeout_style(strikeout_style_t s)
{
    m_cur_font.strikeout_style = s;
}

void import_styles::set_strikeout_width(strikeout_width_t s)
{
    m_cur_font.strikeout_width = s;
}

void import_styles::set_strikeout_type(strikeout_type_t s)
{
    m_cur_font.strikeout_type = s;
}

void import_styles::set_strikeout_text(strikeout_text_t s)
{
    m_cur_font.strikeout_text = s;
}

size_t import_styles::commit_font()
{
    m_fonts.push_back(m_cur_font);
    m_cur_font.reset();
    return m_fonts.size() - 1;
}

void import_styles::set_fill_count(size_t n)
{
    m_fills.reserve(n);
}

void import_styles::set_fill_pattern_type(const char* s, size_t n)
{
    m_cur_fill.pattern_type = m_string_pool.intern(s, n).first;
}

void import_styles::set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_fill.fg_color = color_t(alpha, red, green, blue);
}

void import_styles::set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_fill.bg_color = color_t(alpha, red, green, blue);
}

size_t import_styles::commit_fill()
{
    m_fills.push_back(m_cur_fill);
    m_cur_fill.reset();
    return m_fills.size() - 1;
}

namespace {

border_attrs_t* get_border_attrs(border_t& cur_border, border_direction_t dir)
{
    border_attrs_t* p = NULL;
    switch (dir)
    {
        case border_direction_t::top:
            p = &cur_border.top;
        break;
        case border_direction_t::bottom:
            p = &cur_border.bottom;
        break;
        case border_direction_t::left:
            p = &cur_border.left;
        break;
        case border_direction_t::right:
            p = &cur_border.right;
        break;
        case border_direction_t::diagonal:
            p = &cur_border.diagonal;
        break;
        case border_direction_t::diagonal_bl_tr:
            p = &cur_border.diagonal_bl_tr;
        break;
        case border_direction_t::diagonal_tl_br:
            p = &cur_border.diagonal_tl_br;
        break;
        default:
            ;
    }

    return p;
}

}

void import_styles::set_border_count(size_t n)
{
    m_borders.reserve(n);
}

void import_styles::set_border_style(border_direction_t dir, const char* s, size_t n)
{
    assert(false);
}

void import_styles::set_border_style(border_direction_t dir, border_style_t style)
{
    border_attrs_t* p = get_border_attrs(m_cur_border, dir);
    if (p)
        p->style = style;
}

void import_styles::set_border_color(
    border_direction_t dir, color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    border_attrs_t* p = get_border_attrs(m_cur_border, dir);
    if (p)
        p->border_color = color_t(alpha, red, green, blue);
}

void import_styles::set_border_width(border_direction_t dir, double width, orcus::length_unit_t unit)
{
    border_attrs_t* p = get_border_attrs(m_cur_border, dir);
    if (p)
    {
        p->border_width.value = width;
        p->border_width.unit = unit;
    }
}

size_t import_styles::commit_border()
{
    m_borders.push_back(m_cur_border);
    m_cur_border.reset();
    return m_borders.size() - 1;
}

void import_styles::set_cell_hidden(bool b)
{
    m_cur_protection.hidden = b;
}

void import_styles::set_cell_locked(bool b)
{
    m_cur_protection.locked = b;
}

void import_styles::set_cell_print_content(bool b)
{
    m_cur_protection.print_content = b;
}

void import_styles::set_cell_formula_hidden(bool b)
{
    m_cur_protection.formula_hidden = b;
}

size_t import_styles::commit_cell_protection()
{
    m_protections.push_back(m_cur_protection);
    m_cur_protection.reset();
    return m_protections.size() - 1;
}

void import_styles::set_number_format_count(size_t n)
{
    m_number_formats.reserve(n);
}

void import_styles::set_number_format_identifier(size_t id)
{
    m_cur_number_format.identifier = id;
}

void import_styles::set_number_format_code(const char* s, size_t n)
{
    m_cur_number_format.format_string = pstring(s, n);
}

size_t import_styles::commit_number_format()
{
    m_number_formats.push_back(m_cur_number_format);
    m_cur_number_format.reset();
    return m_number_formats.size() - 1;
}

void import_styles::set_cell_style_xf_count(size_t n)
{
    m_cell_style_formats.reserve(n);
}

size_t import_styles::commit_cell_style_xf()
{
    m_cell_style_formats.push_back(m_cur_cell_format);
    m_cur_cell_format.reset();
    return m_cell_style_formats.size() - 1;
}

void import_styles::set_cell_xf_count(size_t n)
{
    m_cell_formats.reserve(n);
}

size_t import_styles::commit_cell_xf()
{
    m_cell_formats.push_back(m_cur_cell_format);
    m_cur_cell_format.reset();
    return m_cell_formats.size() - 1;
}

void import_styles::set_dxf_count(size_t n)
{
    m_dxf_formats.reserve(n);
}

size_t import_styles::commit_dxf()
{
    m_dxf_formats.push_back(m_cur_cell_format);
    m_cur_cell_format.reset();
    return m_dxf_formats.size() - 1;
}

void import_styles::set_xf_font(size_t index)
{
    m_cur_cell_format.font = index;
}

void import_styles::set_xf_fill(size_t index)
{
    m_cur_cell_format.fill = index;
}

void import_styles::set_xf_border(size_t index)
{
    m_cur_cell_format.border = index;
}

void import_styles::set_xf_protection(size_t index)
{
    m_cur_cell_format.protection = index;
}

void import_styles::set_xf_number_format(size_t index)
{
    m_cur_cell_format.number_format = index;
}

void import_styles::set_xf_style_xf(size_t index)
{
    m_cur_cell_format.style_xf = index;
}

void import_styles::set_xf_apply_alignment(bool b)
{
    m_cur_cell_format.apply_alignment = b;
}

void import_styles::set_xf_horizontal_alignment(orcus::spreadsheet::hor_alignment_t align)
{
    m_cur_cell_format.hor_align = align;
}

void import_styles::set_xf_vertical_alignment(orcus::spreadsheet::ver_alignment_t align)
{
    m_cur_cell_format.ver_align = align;
}

void import_styles::set_cell_style_count(size_t n)
{
    m_cell_styles.reserve(n);
}

void import_styles::set_cell_style_name(const char* s, size_t n)
{
    m_cur_cell_style.name = m_string_pool.intern(s, n).first;
}

void import_styles::set_cell_style_xf(size_t index)
{
    m_cur_cell_style.xf = index;
}

void import_styles::set_cell_style_builtin(size_t index)
{
    m_cur_cell_style.builtin = index;
}

void import_styles::set_cell_style_parent_name(const char* s, size_t n)
{
    m_cur_cell_style.parent_name = m_string_pool.intern(s, n).first;
}

size_t import_styles::commit_cell_style()
{
    m_cell_styles.push_back(m_cur_cell_style);
    m_cur_cell_style.reset();
    return m_cell_styles.size() - 1;
}

const font_t* import_styles::get_font(size_t index) const
{
    if (index >= m_fonts.size())
        return NULL;

    return &m_fonts[index];
}

const cell_format_t* import_styles::get_cell_format(size_t index) const
{
    if (index >= m_cell_formats.size())
        return NULL;

    return &m_cell_formats[index];
}

const fill_t* import_styles::get_fill(size_t index) const
{
    if (index >= m_fills.size())
        return NULL;

    return &m_fills[index];
}

const border_t* import_styles::get_border(size_t index) const
{
    if (index >= m_borders.size())
        return NULL;

    return &m_borders[index];
}

const protection_t* import_styles::get_protection(size_t index) const
{
    if (index >= m_protections.size())
        return NULL;

    return &m_protections[index];
}

const number_format_t* import_styles::get_number_format(size_t index) const
{
    if (index >= m_number_formats.size())
        return NULL;

    return &m_number_formats[index];
}

const cell_format_t* import_styles::get_cell_style_format(size_t index) const
{
    if (index >= m_cell_style_formats.size())
        return NULL;

    return &m_cell_style_formats[index];
}

const cell_format_t* import_styles::get_dxf_format(size_t index) const
{
    if (index >= m_dxf_formats.size())
        return NULL;

    return &m_dxf_formats[index];
}

const cell_style_t* import_styles::get_cell_style(size_t index) const
{
    if (index >= m_cell_styles.size())
        return NULL;

    return &m_cell_styles[index];
}

size_t import_styles::get_font_count() const
{
    return m_fonts.size();
}

size_t import_styles::get_fill_count() const
{
    return m_fills.size();
}

size_t import_styles::get_border_count() const
{
    return m_borders.size();
}

size_t import_styles::get_protection_count() const
{
    return m_protections.size();
}

size_t import_styles::get_number_format_count() const
{
    return m_number_formats.size();
}

size_t import_styles::get_cell_formats_count() const
{
    return m_cell_formats.size();
}

size_t import_styles::get_cell_style_formats_count() const
{
    return m_cell_style_formats.size();
}

size_t import_styles::get_dxf_count() const
{
    return m_dxf_formats.size();
}

size_t import_styles::get_cell_styles_count() const
{
    return m_cell_styles.size();
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
