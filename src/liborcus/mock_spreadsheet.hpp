/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SPREADSHEET_MOCK_IMPORT_INTERFACE_HPP__
#define __ORCUS_SPREADSHEET_MOCK_IMPORT_INTERFACE_HPP__

#include <orcus/spreadsheet/import_interface.hpp>

namespace orcus { namespace spreadsheet { namespace mock {

class import_shared_strings : public orcus::spreadsheet::iface::import_shared_strings
{
public:
    virtual ~import_shared_strings();

    virtual size_t append(const char* s, size_t n);

    virtual size_t add(const char* s, size_t n);

    virtual void set_segment_font(size_t font_index);
    virtual void set_segment_bold(bool b);
    virtual void set_segment_italic(bool b);
    virtual void set_segment_font_name(const char* s, size_t n);
    virtual void set_segment_font_size(double point);
    virtual void set_segment_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void append_segment(const char* s, size_t n);
    virtual size_t commit_segments();
};

class import_styles : public orcus::spreadsheet::iface::import_styles
{
public:
    virtual ~import_styles();

    // font

    virtual void set_font_count(size_t n);
    virtual void set_font_bold(bool b);
    virtual void set_font_italic(bool b);
    virtual void set_font_name(const char* s, size_t n);
    virtual void set_font_size(double point);
    virtual void set_font_underline(orcus::spreadsheet::underline_t e);
    virtual size_t commit_font();

    // fill

    virtual void set_fill_count(size_t n);
    virtual void set_fill_pattern_type(const char* s, size_t n);
    virtual void set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual size_t commit_fill();

    // border

    virtual void set_border_count(size_t n);
    virtual void set_border_style(orcus::spreadsheet::border_direction_t dir, const char* s, size_t n);
    virtual void set_border_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual size_t commit_border();

    // cell protection
    virtual void set_cell_hidden(bool b);
    virtual void set_cell_locked(bool b);
    virtual size_t commit_cell_protection();

    // cell style xf

    virtual void set_cell_style_xf_count(size_t n);
    virtual size_t commit_cell_style_xf();

    // cell xf

    virtual void set_cell_xf_count(size_t n);
    virtual size_t commit_cell_xf();

    // xf (cell format) - used both by cell xf and cell style xf.

    virtual void set_xf_number_format(size_t index);
    virtual void set_xf_font(size_t index);
    virtual void set_xf_fill(size_t index);
    virtual void set_xf_border(size_t index);
    virtual void set_xf_protection(size_t index);
    virtual void set_xf_style_xf(size_t index);
    virtual void set_xf_apply_alignment(bool b);
    virtual void set_xf_horizontal_alignment(orcus::spreadsheet::hor_alignment_t align);
    virtual void set_xf_vertical_alignment(orcus::spreadsheet::ver_alignment_t align);

    // cell style entry

    virtual void set_cell_style_count(size_t n);
    virtual void set_cell_style_name(const char* s, size_t n);
    virtual void set_cell_style_xf(size_t index);
    virtual void set_cell_style_builtin(size_t index);
    virtual size_t commit_cell_style();
};

class import_sheet_properties : public orcus::spreadsheet::iface::import_sheet_properties
{
public:
    virtual ~import_sheet_properties();

    virtual void set_column_width(orcus::spreadsheet::col_t col, double width, orcus::length_unit_t unit);

    virtual void set_column_hidden(orcus::spreadsheet::col_t col, bool hidden);

    virtual void set_row_height(orcus::spreadsheet::row_t row, double height, orcus::length_unit_t unit);

    virtual void set_row_hidden(orcus::spreadsheet::row_t row, bool hidden);

    virtual void set_merge_cell_range(const char* p_ref, size_t p_ref_len);
};

/**
 * Interface for sheet.
 */
class import_sheet : public orcus::spreadsheet::iface::import_sheet
{
public:
    virtual ~import_sheet();

    virtual void set_auto(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, const char* p, size_t n);

    virtual void set_string(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t sindex);

    virtual void set_value(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, double value);

    virtual void set_bool(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, bool value);

    virtual void set_date_time(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col,
        int year, int month, int day, int hours, int minutes, double seconds);

    virtual void set_format(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t xf_index);

    virtual void set_format(orcus::spreadsheet::row_t row_start, orcus::spreadsheet::col_t col_start,
            orcus::spreadsheet::row_t row_end, orcus::spreadsheet::col_t col_end, size_t xf_index);

    virtual void set_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
         const char* p, size_t n);

    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        size_t sindex, const char* p_formula, size_t n_formula, const char* p_range, size_t n_range);

    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        size_t sindex, const char* p_formula, size_t n_formula);

    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t sindex);

    virtual void set_formula_result(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, double value);

    virtual void set_formula_result(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, const char* p, size_t n);

    virtual void set_array_formula(row_t, col_t, formula_grammar_t,
            const char*, size_t, row_t, col_t);

    virtual void set_array_formula(row_t, col_t, formula_grammar_t,
            const char*, size_t, const char*, size_t);
};

class import_factory : public orcus::spreadsheet::iface::import_factory
{
public:
    virtual ~import_factory();

    virtual orcus::spreadsheet::iface::import_global_settings* get_global_settings();

    virtual orcus::spreadsheet::iface::import_shared_strings* get_shared_strings();

    virtual orcus::spreadsheet::iface::import_styles* get_styles();

    virtual orcus::spreadsheet::iface::import_sheet* append_sheet(const char* sheet_name, size_t sheet_name_length);

    virtual orcus::spreadsheet::iface::import_sheet* get_sheet(const char* sheet_name, size_t sheet_name_length);

    virtual orcus::spreadsheet::iface::import_sheet* get_sheet(orcus::spreadsheet::sheet_t sheet_index);

    virtual void finalize();
};

}}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
