/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SPREADSHEET_STYLES_HPP__
#define __ORCUS_SPREADSHEET_STYLES_HPP__

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/pstring.hpp"
#include "orcus/env.hpp"

#include <vector>

namespace orcus {

class string_pool;

namespace spreadsheet {

struct ORCUS_DLLPUBLIC font
{
    pstring name;
    double size;
    bool bold:1;
    bool italic:1;
    underline_t underline;

    font();
    void reset();
};

struct ORCUS_DLLPUBLIC color
{
    color_elem_t alpha;
    color_elem_t red;
    color_elem_t green;
    color_elem_t blue;

    color();
    color(color_elem_t _alpha, color_elem_t _red, color_elem_t _green, color_elem_t _blue);

    void reset();
};

struct ORCUS_DLLPUBLIC fill
{
    pstring pattern_type;
    color fg_color;
    color bg_color;

    fill();
    void reset();
};

struct ORCUS_DLLPUBLIC border_attrs
{
    pstring style;
    color border_color;

    border_attrs();
    void reset();
};

struct ORCUS_DLLPUBLIC border
{
    border_attrs top;
    border_attrs bottom;
    border_attrs left;
    border_attrs right;
    border_attrs diagonal;

    border();
    void reset();
};

struct ORCUS_DLLPUBLIC protection
{
    bool locked;
    bool hidden;

    protection();
    void reset();
};

struct ORCUS_DLLPUBLIC number_format
{
    pstring format_string;

    void reset();

    bool operator==(const number_format& r)
    {
        return format_string == r.format_string;
    }
};

/**
 * Cell format attributes
 */
struct ORCUS_DLLPUBLIC cell_format
{
    size_t font;            /// font ID
    size_t fill;            /// fill ID
    size_t border;          /// border ID
    size_t protection;      /// protection ID
    size_t number_format;   /// number format ID
    size_t style_xf;        /// style XF ID (used only for cell format)
    bool apply_num_format:1;
    bool apply_font:1;
    bool apply_fill:1;
    bool apply_border:1;
    bool apply_alignment:1;

    cell_format();
    void reset();
};

struct ORCUS_DLLPUBLIC cell_style
{
    pstring name;
    size_t xf;
    size_t builtin;

    cell_style();
    void reset();
};

class ORCUS_DLLPUBLIC import_styles : public iface::import_styles
{
public:

    import_styles(string_pool& sp);
    virtual ~import_styles();

    virtual void set_font_count(size_t n);
    virtual void set_font_bold(bool b);
    virtual void set_font_italic(bool b);
    virtual void set_font_name(const char* s, size_t n);
    virtual void set_font_size(double point);
    virtual void set_font_underline(underline_t e);
    virtual size_t commit_font();

    virtual void set_fill_count(size_t n);
    virtual void set_fill_pattern_type(const char* s, size_t n);
    virtual void set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual size_t commit_fill();

    virtual void set_border_count(size_t n);
    virtual void set_border_style(border_direction_t dir, const char* s, size_t n);
    virtual void set_border_color(
        border_direction_t dir, color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual size_t commit_border();

    virtual void set_cell_hidden(bool b);
    virtual void set_cell_locked(bool b);
    virtual size_t commit_cell_protection();

    virtual void set_number_format(const char* s, size_t n);
    virtual size_t commit_number_format();

    virtual void set_cell_style_xf_count(size_t n);
    virtual size_t commit_cell_style_xf();

    virtual void set_cell_xf_count(size_t n);
    virtual size_t commit_cell_xf();

    virtual void set_xf_font(size_t index);
    virtual void set_xf_fill(size_t index);
    virtual void set_xf_border(size_t index);
    virtual void set_xf_protection(size_t index);
    virtual void set_xf_number_format(size_t index);
    virtual void set_xf_style_xf(size_t index);

    virtual void set_cell_style_count(size_t n);
    virtual void set_cell_style_name(const char* s, size_t n);
    virtual void set_cell_style_xf(size_t index);
    virtual void set_cell_style_builtin(size_t index);
    virtual size_t commit_cell_style();

    const font* get_font(size_t index) const;
    const cell_format* get_cell_format(size_t index) const;
    const fill* get_fill(size_t index) const;
    const border* get_border(size_t index) const;

private:
    string_pool& m_string_pool;

    font m_cur_font;
    fill m_cur_fill;
    border m_cur_border;
    protection m_cur_protection;
    number_format m_cur_number_format;
    cell_format m_cur_cell_format;
    cell_style m_cur_cell_style;

    ::std::vector<font> m_fonts;
    ::std::vector<fill> m_fills;
    ::std::vector<border> m_borders;
    ::std::vector<protection> m_protections;
    ::std::vector<number_format> m_number_formats;
    ::std::vector<cell_format> m_cell_style_formats;
    ::std::vector<cell_format> m_cell_formats;
    ::std::vector<cell_style> m_cell_styles;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
