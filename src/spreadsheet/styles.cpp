/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/styles.hpp"
#include "orcus/string_pool.hpp"

#include <algorithm>

namespace orcus { namespace spreadsheet {

font::font() :
    size(0.0), bold(false),
    italic(false), underline(underline_none)
{
}

void font::reset()
{
    *this = font();
}

color::color() :
    alpha(0), red(0), green(0), blue(0)
{
}

color::color(color_elem_t _alpha, color_elem_t _red, color_elem_t _green, color_elem_t _blue) :
    alpha(_alpha), red(_red), green(_green), blue(_blue)
{
}

void color::reset()
{
    *this = color();
}

fill::fill()
{
}

void fill::reset()
{
    *this = fill();
}

border_attrs::border_attrs()
{
}

void border_attrs::reset()
{
    *this = border_attrs();
}

border::border()
{
}

void border::reset()
{
    *this = border();
}

protection::protection() :
    locked(false), hidden(false)
{
}

void protection::reset()
{
    *this = protection();
}

void number_format::reset()
{
    *this = number_format();
}

cell_format::cell_format() :
    font(0),
    fill(0),
    border(0),
    protection(0),
    number_format(0),
    style_xf(0),
    apply_num_format(false),
    apply_font(false),
    apply_fill(false),
    apply_border(false),
    apply_alignment(false)
{
}

void cell_format::reset()
{
    *this = cell_format();
}

cell_style::cell_style() :
    xf(0), builtin(0)
{
}

void cell_style::reset()
{
    *this = cell_style();
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
    m_cur_font.underline = e;
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
    m_cur_fill.fg_color = color(alpha, red, green, blue);
}

void import_styles::set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_fill.bg_color = color(alpha, red, green, blue);
}

size_t import_styles::commit_fill()
{
    m_fills.push_back(m_cur_fill);
    m_cur_fill.reset();
    return m_fills.size() - 1;
}

void import_styles::set_border_count(size_t n)
{
    m_borders.reserve(n);
}

void import_styles::set_border_style(border_direction_t dir, const char* s, size_t n)
{
    border_attrs* p = NULL;
    switch (dir)
    {
        case border_top:
            p = &m_cur_border.top;
        break;
        case border_bottom:
            p = &m_cur_border.bottom;
        break;
        case border_left:
            p = &m_cur_border.left;
        break;
        case border_right:
            p = &m_cur_border.right;
        break;
        case border_diagonal:
            p = &m_cur_border.diagonal;
        break;
    }

    if (p)
        p->style = m_string_pool.intern(s, n).first;
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

size_t import_styles::commit_cell_protection()
{
    m_protections.push_back(m_cur_protection);
    m_cur_protection.reset();
    return m_protections.size() - 1;
}

void import_styles::set_number_format(const char* s, size_t n)
{
    m_cur_number_format.format_string = pstring(s, n);
}

size_t import_styles::commit_number_format()
{
    std::vector<number_format>::iterator itr = std::find(m_number_formats.begin(), m_number_formats.end(), m_cur_number_format);
    if(itr != m_number_formats.end())
    {
        m_cur_number_format.reset();
        return std::distance(m_number_formats.begin(), itr);
    }

    m_number_formats.push_back(m_cur_number_format);
    return m_number_formats.size () - 1;
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

size_t import_styles::commit_cell_style()
{
    m_cell_styles.push_back(m_cur_cell_style);
    m_cur_cell_style.reset();
    return m_cell_styles.size() - 1;
}

const font* import_styles::get_font(size_t index) const
{
    if (index >= m_fonts.size())
        return NULL;

    return &m_fonts[index];
}

const cell_format* import_styles::get_cell_format(size_t index) const
{
    if (index >= m_cell_formats.size())
        return NULL;

    return &m_cell_formats[index];
}

const fill* import_styles::get_fill(size_t index) const
{
    if (index >= m_fills.size())
        return NULL;

    return &m_fills[index];
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
