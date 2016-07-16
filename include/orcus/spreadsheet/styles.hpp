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
#include <orcus/measurement.hpp>
#include <vector>

namespace orcus {

class string_pool;

namespace spreadsheet {

struct ORCUS_SPM_DLLPUBLIC color_t
{
    color_elem_t alpha;
    color_elem_t red;
    color_elem_t green;
    color_elem_t blue;

    color_t();
    color_t(color_elem_t _alpha, color_elem_t _red, color_elem_t _green, color_elem_t _blue);

    void reset();
};

struct ORCUS_SPM_DLLPUBLIC font_t
{
    pstring name;
    double size;
    bool bold:1;
    bool italic:1;
    underline_t underline_style;
    underline_width_t underline_width;
    underline_mode_t underline_mode;
    underline_type_t underline_type;
    color_t underline_color;
    color_t color;
    strikeout_style_t strikeout_style;
    strikeout_width_t strikeout_width;
    strikeout_type_t strikeout_type;
    strikeout_text_t strikeout_text;

    font_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC fill_t
{
    pstring pattern_type;
    color_t fg_color;
    color_t bg_color;

    fill_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC border_attrs_t
{
    border_style_t style;
    color_t border_color;
    length_t border_width;

    border_attrs_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC border_t
{
    border_attrs_t top;
    border_attrs_t bottom;
    border_attrs_t left;
    border_attrs_t right;
    border_attrs_t diagonal;
    border_attrs_t diagonal_bl_tr;
    border_attrs_t diagonal_tl_br;

    border_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC protection_t
{
    bool locked;
    bool hidden;
    bool print_content;
    bool formula_hidden;

    protection_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC number_format_t
{
    size_t identifier;
    pstring format_string;

    number_format_t();
    void reset();
    bool operator== (const number_format_t& r) const;
};

/**
 * Cell format attributes
 */
struct ORCUS_SPM_DLLPUBLIC cell_format_t
{
    size_t font;            /// font ID
    size_t fill;            /// fill ID
    size_t border;          /// border ID
    size_t protection;      /// protection ID
    size_t number_format;   /// number format ID
    size_t style_xf;        /// style XF ID (used only for cell format)
    hor_alignment_t hor_align;
    ver_alignment_t ver_align;
    bool apply_num_format:1;
    bool apply_font:1;
    bool apply_fill:1;
    bool apply_border:1;
    bool apply_alignment:1;
    bool apply_protection:1;

    cell_format_t();
    void reset();
};

struct ORCUS_SPM_DLLPUBLIC cell_style_t
{
    pstring name;
    size_t xf;
    size_t builtin;
    pstring parent_name;

    cell_style_t();
    void reset();
};

class ORCUS_SPM_DLLPUBLIC import_styles : public iface::import_styles
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
    virtual void set_font_underline_width(underline_width_t e);
    virtual void set_font_underline_mode(underline_mode_t e);
    virtual void set_font_underline_type(underline_type_t e);
    virtual void set_font_underline_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_strikeout_style(strikeout_style_t s);
    virtual void set_strikeout_type(strikeout_type_t s);
    virtual void set_strikeout_width(strikeout_width_t s);
    virtual void set_strikeout_text(strikeout_text_t s);
    virtual size_t commit_font();

    virtual void set_fill_count(size_t n);
    virtual void set_fill_pattern_type(const char* s, size_t n);
    virtual void set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual size_t commit_fill();

    virtual void set_border_count(size_t n);
    virtual void set_border_style(border_direction_t dir, const char* s, size_t n);
    virtual void set_border_style(border_direction_t dir, border_style_t style);
    virtual void set_border_color(
        border_direction_t dir, color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_border_width(border_direction_t dir, double width, orcus::length_unit_t unit);
    virtual size_t commit_border();

    virtual void set_cell_hidden(bool b);
    virtual void set_cell_locked(bool b);
    virtual void set_cell_print_content(bool b);
    virtual void set_cell_formula_hidden(bool b);
    virtual size_t commit_cell_protection();

    virtual void set_number_format_count(size_t n);
    virtual void set_number_format_identifier(size_t id);
    virtual void set_number_format_code(const char* s, size_t n);
    virtual size_t commit_number_format();

    virtual void set_cell_style_xf_count(size_t n);
    virtual size_t commit_cell_style_xf();

    virtual void set_cell_xf_count(size_t n);
    virtual size_t commit_cell_xf();

    virtual void set_dxf_count(size_t n);
    virtual size_t commit_dxf();

    virtual void set_xf_font(size_t index);
    virtual void set_xf_fill(size_t index);
    virtual void set_xf_border(size_t index);
    virtual void set_xf_protection(size_t index);
    virtual void set_xf_number_format(size_t index);
    virtual void set_xf_style_xf(size_t index);
    virtual void set_xf_apply_alignment(bool b);
    virtual void set_xf_horizontal_alignment(orcus::spreadsheet::hor_alignment_t align);
    virtual void set_xf_vertical_alignment(orcus::spreadsheet::ver_alignment_t align);

    virtual void set_cell_style_count(size_t n);
    virtual void set_cell_style_name(const char* s, size_t n);
    virtual void set_cell_style_xf(size_t index);
    virtual void set_cell_style_builtin(size_t index);
    virtual void set_cell_style_parent_name(const char* s, size_t n);
    virtual size_t commit_cell_style();

    const font_t* get_font(size_t index) const;
    const fill_t* get_fill(size_t index) const;
    const border_t* get_border(size_t index) const;
    const protection_t* get_protection(size_t index) const;
    const number_format_t* get_number_format(size_t index) const;
    const cell_format_t* get_cell_format(size_t index) const;
    const cell_format_t* get_cell_style_format(size_t index) const;
    const cell_format_t* get_dxf_format(size_t index) const;
    const cell_style_t* get_cell_style(size_t index) const;

    size_t get_font_count() const;
    size_t get_fill_count() const;
    size_t get_border_count() const;
    size_t get_protection_count() const;
    size_t get_number_format_count() const;
    size_t get_cell_formats_count() const;
    size_t get_cell_style_formats_count() const;
    size_t get_dxf_count() const;
    size_t get_cell_styles_count() const;

private:
    string_pool& m_string_pool;

    font_t m_cur_font;
    fill_t m_cur_fill;
    border_t m_cur_border;
    protection_t m_cur_protection;
    number_format_t m_cur_number_format;
    cell_format_t m_cur_cell_format;
    cell_style_t m_cur_cell_style;

    ::std::vector<font_t> m_fonts;
    ::std::vector<fill_t> m_fills;
    ::std::vector<border_t> m_borders;
    ::std::vector<protection_t> m_protections;
    ::std::vector<number_format_t> m_number_formats;
    ::std::vector<cell_format_t> m_cell_style_formats;
    ::std::vector<cell_format_t> m_cell_formats;
    ::std::vector<cell_format_t> m_dxf_formats;
    ::std::vector<cell_style_t> m_cell_styles;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
