/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_IMPORT_INTERFACE_HPP
#define ORCUS_SPREADSHEET_IMPORT_INTERFACE_HPP

#include <cstdlib>

#include "orcus/spreadsheet/types.hpp"
#include "orcus/types.hpp"
#include "orcus/env.hpp"

// NB: This header must not depend on ixion, as it needs to be usable for
// those clients that provide their own formula engine.  Other headers in
// the orcus::spreadsheet namespace may depend on ixion.

namespace orcus { namespace spreadsheet { namespace iface {

/**
 * Interface class designed to be derived by the implementor.
 */
class import_shared_strings
{
public:
    ORCUS_DLLPUBLIC virtual ~import_shared_strings() = 0;

    /**
     * Append new string to the string list.  Order of insertion is important
     * since that determines the numerical ID values of inserted strings.
     * Note that this method assumes that the caller knows the string being
     * appended is not yet in the pool.
     *
     * @param s pointer to the first character of the string array.  The
     *          string array doesn't necessary have to be null-terminated.
     * @param n length of the string.
     *
     * @return ID of the string just inserted.
     */
    virtual size_t append(const char* s, size_t n) = 0;

    /**
     * Similar to the append method, it adds new string to the string pool;
     * however, this method checks if the string being added is already in the
     * pool before each insertion, to avoid duplicated strings.
     *
     * @param s pointer to the first character of the string array.  The
     *          string array doesn't necessary have to be null-terminated.
     * @param n length of the string.
     *
     * @return ID of the string just inserted.
     */
    virtual size_t add(const char* s, size_t n) = 0;

    virtual void set_segment_font(size_t font_index) = 0;
    virtual void set_segment_bold(bool b) = 0;
    virtual void set_segment_italic(bool b) = 0;
    virtual void set_segment_font_name(const char* s, size_t n) = 0;
    virtual void set_segment_font_size(double point) = 0;
    virtual void set_segment_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void append_segment(const char* s, size_t n) = 0;
    virtual size_t commit_segments() = 0;
};

/**
 * Interface for styles. Note that because the default style must have an
 * index of 0 in each style category, the caller must set the default styles
 * first before importing and setting real styles. ID's of styles are
 * assigned sequentially starting with 0 and upward in each style category.
 *
 * In contrast to xf formatting, dxf (differential formats) formatting only
 * stores the format information that is explicitly set. It does not store
 * formatting from the default style. Applying a dxf format to an object
 * only applies those explicitly set formats from the dxf entry, while all
 * the other formats are retained.
 */
class import_styles
{
public:
    ORCUS_DLLPUBLIC virtual ~import_styles() = 0;

    // font

    virtual void set_font_count(size_t n) = 0;
    virtual void set_font_bold(bool b) = 0;
    virtual void set_font_italic(bool b) = 0;
    virtual void set_font_name(const char* s, size_t n) = 0;
    virtual void set_font_size(double point) = 0;
    virtual void set_font_underline(orcus::spreadsheet::underline_t e) = 0;
    virtual void set_font_underline_width(underline_width_t e) = 0;
    virtual void set_font_underline_mode(underline_mode_t e) = 0;
    virtual void set_font_underline_type(underline_type_t e) = 0;
    virtual void set_font_underline_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void set_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void set_strikeout_style(orcus::spreadsheet::strikeout_style_t s) = 0;
    virtual void set_strikeout_type(orcus::spreadsheet::strikeout_type_t s) = 0;
    virtual void set_strikeout_width(orcus::spreadsheet::strikeout_width_t s) = 0;
    virtual void set_strikeout_text(orcus::spreadsheet::strikeout_text_t s) = 0;
    virtual size_t commit_font() = 0;

    // fill

    virtual void set_fill_count(size_t n) = 0;
    virtual void set_fill_pattern_type(const char* s, size_t n) = 0;
    virtual void set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual size_t commit_fill() = 0;

    // border

    virtual void set_border_count(size_t n) = 0;

    /// @deprecated: use set_border_style(border_direction, border_style_t) instead
    virtual void set_border_style(orcus::spreadsheet::border_direction_t dir, const char* s, size_t n) = 0;
    virtual void set_border_style(orcus::spreadsheet::border_direction_t dir, border_style_t style) = 0;
    virtual void set_border_color(
        orcus::spreadsheet::border_direction_t dir, color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void set_border_width(border_direction_t dir, double width, orcus::length_unit_t unit) = 0;
    virtual size_t commit_border() = 0;

    // cell protection
    virtual void set_cell_hidden(bool b) = 0;
    virtual void set_cell_locked(bool b) = 0;
    virtual void set_cell_print_content(bool b) = 0;
    virtual void set_cell_formula_hidden(bool b) = 0;
    virtual size_t commit_cell_protection() = 0;

    // number format
    virtual void set_number_format_count(size_t n) = 0;
    virtual void set_number_format_identifier(size_t id) = 0;
    virtual void set_number_format_code(const char* s, size_t n) = 0;
    virtual size_t commit_number_format() = 0;

    // cell format and cell style format (xf == cell format)

    virtual void set_cell_xf_count(size_t n) = 0;
    virtual void set_cell_style_xf_count(size_t n) = 0;
    virtual void set_dxf_count(size_t n) = 0;

    virtual void set_xf_font(size_t index) = 0;
    virtual void set_xf_fill(size_t index) = 0;
    virtual void set_xf_border(size_t index) = 0;
    virtual void set_xf_protection(size_t index) = 0;
    virtual void set_xf_number_format(size_t index) = 0;
    virtual void set_xf_style_xf(size_t index) = 0;
    virtual void set_xf_apply_alignment(bool b) = 0;
    virtual void set_xf_horizontal_alignment(orcus::spreadsheet::hor_alignment_t align) = 0;
    virtual void set_xf_vertical_alignment(orcus::spreadsheet::ver_alignment_t align) = 0;

    virtual size_t commit_cell_xf() = 0;
    virtual size_t commit_cell_style_xf() = 0;
    virtual size_t commit_dxf() = 0;

    // cell style entry

    virtual void set_cell_style_count(size_t n) = 0;
    virtual void set_cell_style_name(const char* s, size_t n) = 0;
    virtual void set_cell_style_xf(size_t index) = 0;
    virtual void set_cell_style_builtin(size_t index) = 0;
    virtual void set_cell_style_parent_name(const char* s, size_t n) = 0;
    virtual size_t commit_cell_style() = 0;
};

class import_sheet_properties
{
public:
    ORCUS_DLLPUBLIC virtual ~import_sheet_properties() = 0;

    virtual void set_column_width(orcus::spreadsheet::col_t col, double width, orcus::length_unit_t unit) = 0;

    virtual void set_column_hidden(orcus::spreadsheet::col_t col, bool hidden) = 0;

    virtual void set_row_height(orcus::spreadsheet::row_t row, double height, orcus::length_unit_t unit) = 0;

    virtual void set_row_hidden(orcus::spreadsheet::row_t row, bool hidden) = 0;

    /**
     * Specify merged cell range.  The range is given in a 2-dimensional
     * A1-style reference.
     *
     * @param p_range pointer to the first character of reference string.
     * @param n_range length of reference string.
     */
    virtual void set_merge_cell_range(const char* p_range, size_t n_range) = 0;
};

/**
 * Interface for importing data tables.
 */
class import_data_table
{
public:
    ORCUS_DLLPUBLIC virtual ~import_data_table() = 0;

    virtual void set_type(orcus::spreadsheet::data_table_type_t type) = 0;

    virtual void set_range(const char* p_range, size_t n_range) = 0;

    virtual void set_first_reference(const char* p_ref, size_t n_ref, bool deleted) = 0;

    virtual void set_second_reference(const char* p_ref, size_t n_ref, bool deleted) = 0;

    virtual void commit() = 0;
};

class import_auto_filter
{
public:
    ORCUS_DLLPUBLIC virtual ~import_auto_filter() = 0;

    /**
     * Specify the range where the auto filter is applied.  The range
     * is given in a 2-dimensional A1-style reference.
     *
     * @param p_ref pointer to the first character of range string.
     * @param n_ref length of range string.
     */
    virtual void set_range(const char* p_ref, size_t n_ref) = 0;

    /**
     * Specify the column position of a filter. The position is relative to
     * the first column in the auto filter range.
     *
     * @param col 0-based column position of a filter relative to the first
     *            column.
     */
    virtual void set_column(orcus::spreadsheet::col_t col) = 0;

    /**
     * Add a match value to the current column filter.
     *
     * @param p pointer to the first character of match value.
     * @param n length of match value.
     */
    virtual void append_column_match_value(const char* p, size_t n) = 0;

    /**
     * Commit current column filter to the current auto filter.
     */
    virtual void commit_column() = 0;

    /**
     * Commit current auto filter to the model.
     */
    virtual void commit() = 0;
};

/**
 * This is an optional interface to import conditional formatting.
 *
 * A conditional format consists of:
 * <ul>
 *  <li>a range</li>
 *  <li>several entries</li>
 * </ul>
 *
 * Each entry consists of:
 * <ul>
 *   <li>a type</li>
 *   <li>a few properties depending on the type (optional)</li>
 *   <li>zero or more conditions depending on the type</li>
 * </ul>
 *
 * Each condition consists of:
 * <ul>
 *   <li>a formula/value/string</li>
 *   <li>a color (optional)</li>
 * </ul>
 */
class import_conditional_format
{
public:
    ORCUS_DLLPUBLIC virtual ~import_conditional_format() = 0;

    /**
     * Sets the color of the current condition.
     * only valid for type == databar or type == colorscale.
     */
    virtual void set_color(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue) = 0;

    /**
     * Sets the formula, value or string of the current condition.
     */
    virtual void set_formula(const char* p, size_t n) = 0;

    /**
     * Sets the type for the formula, value or string of the current condition.
     * Only valid for type = iconset, databar or colorscale.
     */
    virtual void set_condition_type(orcus::spreadsheet::condition_type_t type) = 0;

    /**
     * Only valid for type = date.
     */
    virtual void set_date(orcus::spreadsheet::condition_date_t date) = 0;

    /**
     * commits the current condition to the current entry.
     */
    virtual void commit_condition() = 0;

    /**
     * Name of the icons to use in the current entry.
     * only valid for type = iconset
     */
    virtual void set_icon_name(const char* p, size_t n) = 0;

    /**
     * Use a gradient for the current entry.
     * only valid for type == databar
     */
    virtual void set_databar_gradient(bool gradient) = 0;

    /**
     * Position of the 0 axis in the current entry.
     * only valid for type == databar.
     */
    virtual void set_databar_axis(orcus::spreadsheet::databar_axis_t axis) = 0;

    /**
     * Databar color for positive values.
     * only valid for type == databar.
     */
    virtual void set_databar_color_positive(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue) = 0;

    /**
     * Databar color for negative values.
     * only valid for type == databar.
     */
    virtual void set_databar_color_negative(color_elem_t alpha, color_elem_t red,
            color_elem_t green, color_elem_t blue) = 0;

    /**
     * Sets the minimum length for a databar.
     * only valid for type == databar.
     */
    virtual void set_min_databar_length(double length) = 0;

    /**
     * Sets the maximum length for a databar.
     * only valid for type == databar.
     */
    virtual void set_max_databar_length(double length) = 0;

    /**
     * Don't show the value in the cell.
     * only valid for type = databar, iconset, colorscale.
     */
    virtual void set_show_value(bool show) = 0;

    /**
     * Use the icons in reverse order.
     * only valid for type == iconset.
     */
    virtual void set_iconset_reverse(bool reverse) = 0;

    /**
     * TODO: In OOXML the style is stored as dxf and in ODF as named style.
     */
    virtual void set_xf_id(size_t xf) = 0;

    /**
     * Sets the current operation used for the current entry.
     * only valid for type == condition
     */
    virtual void set_operator(orcus::spreadsheet::condition_operator_t condition_type) = 0;

    virtual void set_type(orcus::spreadsheet::conditional_format_t type) = 0;

    virtual void commit_entry() = 0;

    virtual void set_range(const char* p, size_t n) = 0;

    virtual void set_range(orcus::spreadsheet::row_t row_start, orcus::spreadsheet::col_t col_start,
            orcus::spreadsheet::row_t row_end, orcus::spreadsheet::col_t col_end) = 0;

    virtual void commit_format() = 0;
};

/**
 * Interface for table.  A table is a range within a sheet that consists of
 * one or more data columns with a header row that contains their labels.
 */
class ORCUS_DLLPUBLIC import_table
{
public:
    virtual ~import_table() = 0;

    virtual import_auto_filter* get_auto_filter();

    virtual void set_identifier(size_t id) = 0;

    virtual void set_range(const char* p_ref, size_t n_ref) = 0;

    virtual void set_totals_row_count(size_t row_count) = 0;

    virtual void set_name(const char* p, size_t n) = 0;

    virtual void set_display_name(const char* p, size_t n) = 0;

    virtual void set_column_count(size_t n) = 0;

    virtual void set_column_identifier(size_t id) = 0;
    virtual void set_column_name(const char* p, size_t n) = 0;
    virtual void set_column_totals_row_label(const char* p, size_t n) = 0;
    virtual void set_column_totals_row_function(orcus::spreadsheet::totals_row_function_t func) = 0;
    virtual void commit_column() = 0;

    virtual void set_style_name(const char* p, size_t n) = 0;
    virtual void set_style_show_first_column(bool b) = 0;
    virtual void set_style_show_last_column(bool b) = 0;
    virtual void set_style_show_row_stripes(bool b) = 0;
    virtual void set_style_show_column_stripes(bool b) = 0;

    virtual void commit() = 0;
};

/**
 * Interface for sheet.
 */
class ORCUS_DLLPUBLIC import_sheet
{
public:
    virtual ~import_sheet() = 0;

    virtual import_sheet_properties* get_sheet_properties();

    /**
     * Get an interface for importing data tables.  Note that the implementer
     * may decide not to support this feature in which case this method
     * returns NULL.  The implementer is responsible for managing the life
     * cycle of the returned interface object.
     *
     * The implementor should also initialize the internal state of the
     * temporary data table object when this method is called.
     *
     * @return pointer to the data table interface object.
     */
    virtual import_data_table* get_data_table();

    /**
     * Get an interface for importing auto filter ranges.
     *
     * The implementor should also initialize the internal state of the
     * temporary auto filter object when this method is called.
     *
     * @return pointer to the auto filter interface object.
     */
    virtual import_auto_filter* get_auto_filter();

    /**
     * Get an interface for importing tables.  The implementer is responsible
     * for managing the life cycle of the returned interface object.
     *
     * The implementor should also initialize the internal state of the
     * temporary table object when this method is called.
     *
     * @return pointer to the table interface object, or NULL if the
     *         implementer doesn't support importing of tables.
     */
    virtual import_table* get_table();

    /**
     * get an interface for importing conditional formats. The implementer is responsible
     * for managing the life cycle of the returned interface object.
     *
     * @return pointer to the conditional format interface object, or NULL
     *          if the implementer doesn't support importing conditional formats.
     */
    virtual import_conditional_format* get_conditional_format();

    /**
     * Set raw string value to a cell and have the implementation
     * auto-recognize its data type.
     *
     * @param row row ID
     * @param col column ID
     * @param p pointer to the first character of the raw string value.
     * @param n size of the raw string value.
     */
    virtual void set_auto(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, const char* p, size_t n) = 0;

    /**
     * Set string value to a cell.
     *
     * @param row row ID
     * @param col column ID
     * @param sindex 0-based string index in the shared string table.
     */
    virtual void set_string(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t sindex) = 0;

    /**
     * Set numerical value to a cell.
     *
     * @param row row ID
     * @param col column ID
     * @param value value being assigned to the cell.
     */
    virtual void set_value(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, double value) = 0;

    /**
     * Set a boolean value to a cell.
     *
     * @param row row ID
     * @param col col ID
     * @param value boolean value being assigned to the cell
     */
    virtual void set_bool(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, bool value) = 0;

    /**
     * Set date and time value to a cell.
     *
     * @param row row ID
     * @param col column ID
     */
    virtual void set_date_time(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col,
        int year, int month, int day, int hour, int minute, double second) = 0;

    /**
     * Set cell format to specified cell.  The cell format is referred to by
     * the xf (cell format) index in the styles table.
     *
     * @param row row ID
     * @param col column ID
     * @param index 0-based xf (cell format) index
     */
    virtual void set_format(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t xf_index) = 0;

    /**
     * Set cell format to specified cell range.  The cell format is referred
     * to by the xf (cell format) index in the styles table.
     *
     * @param row_start start row ID
     * @param col_start start column ID
     * @param row_end end row ID
     * @param col_end end column ID
     * @param index 0-based xf (cell format) index
     */
    virtual void set_format(orcus::spreadsheet::row_t row_start, orcus::spreadsheet::col_t col_start,
        orcus::spreadsheet::row_t row_end, orcus::spreadsheet::col_t col_end, size_t xf_index) = 0;

    /**
     * Set normal, non-shared formula expression to specified cell.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param p pointer to the first character of the raw formula expression
     *          string.
     * @param n size of the raw formula expression string.
     */
    virtual void set_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        const char* p, size_t n) = 0;

    /**
     * Set shared formula expression to specified cell.  This call also
     * registers the formula expression to a common formula expression pool to
     * allow it to be looked up by the specified index later.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param sindex shared formula index (0-based)
     * @param p_formula pointer to the first character of the raw formula
     *          expression string.
     * @param n_formula size of the raw formula expression string.
     * @param p_range pointer to the first character of the range string
     * @param n_range size of the raw range string
     */
    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        size_t sindex, const char* p_formula, size_t n_formula, const char* p_range, size_t n_range) = 0;

    /**
     * Set shared formula expression to specified cell.  This call also
     * registers the formula expression to a common formula expression pool to
     * allow it to be looked up by the specified index later.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param sindex shared formula index (0-based)
     * @param p_formula pointer to the first character of the raw formula
     *          expression string.
     * @param n_formula size of the raw formula expression string.
     */
    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        size_t sindex, const char* p_formula, size_t n_formula) = 0;

    /**
     * Set shared formula to specified cell by shared formula index.  The
     * formula expression itself associated with the index must be defined.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param sindex shared formula index (0-based)
     * @param p pointer to the first character of the raw formula expression
     *          string.
     * @param n size of the raw formula expression string.
     */
    virtual void set_shared_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t sindex) = 0;

    virtual void set_formula_result(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, double value) = 0;

    virtual void set_formula_result(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, const char* p, size_t n) = 0;

    /**
     * Set a array formula to the specified cells. The formula covers an area
     * specified by array_rows and array_cols beginning from the base cell.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param p pointer to the first character of the raw formula expression string.
     * @param n size of the raw formula expression string
     * @param array_rows number of rows the array formula covers
     * @param array_cols number of columns the array formula covers
     */
    virtual void set_array_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        const char* p, size_t n, orcus::spreadsheet::row_t array_rows, orcus::spreadsheet::col_t array_cols) = 0;

    /**
     * Set a array formula to the specified cells. The formula covers an area
     * specified by the range string.
     *
     * @param row row ID
     * @param col column ID
     * @param grammar grammar used in the formula expression
     * @param p pointer to the first character of the raw formula expression string.
     * @param n size of the raw formula expression string
     * @param p_range pointer to the first character of the range string
     * @param n_range isze of the range string
     */
    virtual void set_array_formula(
        orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, orcus::spreadsheet::formula_grammar_t grammar,
        const char* p, size_t n, const char* p_range, size_t n_range) = 0;
};

class import_global_settings
{
public:
    ORCUS_DLLPUBLIC virtual ~import_global_settings() = 0;

    /**
     * Set the date that is to be represented by a value of 0.  All date
     * values will be internally represented relative to this date afterward.
     *
     * @param year 1-based value representing year
     * @param month 1-based value representing month, varying from 1 through
     *              12.
     * @param day 1-based value representing day, varying from 1 through 31.
     */
    virtual void set_origin_date(int year, int month, int day) = 0;

    /**
     * Set formula grammar to be used globally when parsing formulas if the
     * grammar is not specified.  This grammar will also be used when parsing
     * range strings associated with shared formula ranges, array formula
     * ranges, autofilter ranges etc.
     *
     * @param grammar default formula grammar
     */
    virtual void set_default_formula_grammar(orcus::spreadsheet::formula_grammar_t grammar) = 0;

    /**
     * Get current default formula grammar.
     *
     * @return current default formula grammar.
     */
    virtual orcus::spreadsheet::formula_grammar_t get_default_formula_grammar() const = 0;
};

/**
 * This interface provides the filters a means to instantiate concrete
 * classes that implement the above interfaces.  The client code never has
 * to manually delete objects returned by its methods; the implementor of
 * this interface must manage the life cycles of objects it returns.
 *
 * The implementor of this interface normally wraps the document instance
 * inside it and have the document instance manage the life cycles of
 * various objects it creates.
 */
class ORCUS_DLLPUBLIC import_factory
{
public:
    virtual ~import_factory() = 0;

    virtual import_global_settings* get_global_settings();

    /**
     * @return pointer to the shared strings instance. It may return NULL if
     *         the client app doesn't support shared strings.
     */
    virtual import_shared_strings* get_shared_strings();

    /**
     * @return pointer to the styles instance. It may return NULL if the
     *         client app doesn't support styles.
     */
    virtual import_styles* get_styles();

    /**
     * @return pointer to the sheet instance. It may return NULL if the client
     *         app fails to append new sheet.
     */
    virtual import_sheet* append_sheet(const char* sheet_name, size_t sheet_name_length) = 0;

    /**
     * @return pointer to the sheet instance whose name matches the name
     *         passed to this method. It returns NULL if no sheet instance
     *         exists by the specified name.
     */
    virtual import_sheet* get_sheet(const char* sheet_name, size_t sheet_name_length) = 0;

    /**
     * Retrieve sheet instance by specified numerical sheet index.
     *
     * @param sheet_index sheet index
     *
     * @return pointer to the sheet instance, or NULL if no sheet instance
     *         exists at specified sheet index position.
     */
    virtual import_sheet* get_sheet(orcus::spreadsheet::sheet_t sheet_index) = 0;

    /**
     * This method is called at the end of import, to give the implementor a
     * chance to perform post-processing if necessary.
     */
    virtual void finalize() = 0;
};

}}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
