/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#include "styles.hpp"

namespace orcus { namespace model {

styles::font::font() :
    size(0.0), bold(false), italic(false)
{
}

void styles::font::reset()
{
    *this = font();
}

styles::color::color() :
    alpha(0), red(0), green(0), blue(0)
{
}

styles::color::color(uint8_t _alpha, uint8_t _red, uint8_t _green, uint8_t _blue) :
    alpha(_alpha), red(_red), green(_green), blue(_blue)
{
}

void styles::color::reset()
{
    *this = color();
}

styles::fill::fill()
{
}

void styles::fill::reset()
{
    *this = fill();
}

styles::border_attrs::border_attrs()
{
}

void styles::border_attrs::reset()
{
    *this = border_attrs();
}

styles::border::border()
{
}

void styles::border::reset()
{
    *this = border();
}

styles::xf::xf() :
    num_format(0),
    font(0),
    fill(0),
    border(0),
    style_xf(0),
    apply_num_format(false),
    apply_font(false),
    apply_fill(false),
    apply_border(false),
    apply_alignment(false)
{
}

void styles::xf::reset()
{
    *this = xf();
}

styles::cell_style::cell_style() :
    xf(0), builtin(0)
{
}

void styles::cell_style::reset()
{
    *this = cell_style();
}

styles::styles()
{
}

styles::~styles()
{
}

void styles::set_font_count(size_t n)
{
    m_fonts.reserve(n);
}

void styles::set_font_bold(bool b)
{
    m_cur_font.bold = b;
}

void styles::set_font_italic(bool b)
{
    m_cur_font.italic = b;
}

void styles::set_font_name(const char* s, size_t n)
{
    m_cur_font.name = pstring(s, n).intern();
}

void styles::set_font_size(double point)
{
    m_cur_font.size = point;
}

void styles::commit_font()
{
    m_fonts.push_back(m_cur_font);
    m_cur_font.reset();
}

void styles::set_fill_count(size_t n)
{
    m_fills.reserve(n);
}

void styles::set_fill_pattern_type(const char* s, size_t n)
{
    m_cur_fill.pattern_type = pstring(s, n).intern();
}

void styles::set_fill_fg_color(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    m_cur_fill.fg_color = color(alpha, red, green, blue);
}

void styles::set_fill_bg_color(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    m_cur_fill.bg_color = color(alpha, red, green, blue);
}

void styles::commit_fill()
{
    m_fills.push_back(m_cur_fill);
    m_cur_fill.reset();
}

void styles::set_border_count(size_t n)
{
    m_borders.reserve(n);
}

void styles::set_border_style(border_direction_t dir, const char* s, size_t n)
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
        p->style = pstring(s, n).intern();
}

void styles::commit_border()
{
    m_borders.push_back(m_cur_border);
    m_cur_border.reset();
}

void styles::set_cell_style_xf_count(size_t n)
{
    m_cell_style_formats.reserve(n);
}

void styles::commit_cell_style_xf()
{
    m_cell_style_formats.push_back(m_cur_cell_format);
    m_cur_cell_format.reset();
}

void styles::set_cell_xf_count(size_t n)
{
    m_cell_formats.reserve(n);
}

void styles::commit_cell_xf()
{
    m_cell_formats.push_back(m_cur_cell_format);
    m_cur_cell_format.reset();
}

void styles::set_xf_number_format(size_t index)
{
    m_cur_cell_format.num_format = index;
}

void styles::set_xf_font(size_t index)
{
    m_cur_cell_format.font = index;
}

void styles::set_xf_fill(size_t index)
{
    m_cur_cell_format.fill = index;
}

void styles::set_xf_border(size_t index)
{
    m_cur_cell_format.border = index;
}

void styles::set_xf_style_xf(size_t index)
{
    m_cur_cell_format.style_xf = index;
}

void styles::set_cell_style_count(size_t n)
{
    m_cell_styles.reserve(n);
}

void styles::set_cell_style_name(const char* s, size_t n)
{
    m_cur_cell_style.name = pstring(s, n).intern();
}

void styles::set_cell_style_xf(size_t index)
{
    m_cur_cell_style.xf = index;
}

void styles::set_cell_style_builtin(size_t index)
{
    m_cur_cell_style.builtin = index;
}

void styles::commit_cell_style()
{
    m_cell_styles.push_back(m_cur_cell_style);
    m_cur_cell_style.reset();
}

const styles::font* styles::get_font(size_t index) const
{
    if (index >= m_fonts.size())
        return NULL;

    return &m_fonts[index];
}

const styles::xf* styles::get_cell_xf(size_t index) const
{
    if (index >= m_cell_formats.size())
        return NULL;

    return &m_cell_formats[index];
}

const styles::fill* styles::get_fill(size_t index) const
{
    if (index >= m_fills.size())
        return NULL;

    return &m_fills[index];
}

}}
