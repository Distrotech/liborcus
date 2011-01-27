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

#include "orcus/model/styles.hpp"

namespace orcus { namespace model {

styles::font::font() :
    size(0.0), bold(false), italic(false)
{
}

void styles::font::reset()
{
    name.clear();
    size = 0.0;
    bold = false;
    italic = false;
}

styles::fill::fill()
{
}

void styles::fill::reset()
{
    pattern_type.clear();
}

styles::border_attrs::border_attrs()
{
}

void styles::border_attrs::reset()
{
    style.clear();
}

styles::border::border()
{
}

void styles::border::reset()
{
    top.reset();
    bottom.reset();
    left.reset();
    right.reset();
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

const styles::font* styles::get_font(size_t index) const
{
    if (index >= m_fonts.size())
        return NULL;

    return &m_fonts[index];
}

}}
