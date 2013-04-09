/*************************************************************************
 *
 * Copyright (c) 2011-2012 Kohei Yoshida
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

#ifndef __ORCUS_SPREADSHEET_IMPORT_INTERFACE_HPP__
#define __ORCUS_SPREADSHEET_IMPORT_INTERFACE_HPP__

#include <cstdlib>

#include "types.hpp"
#include "../env.hpp"

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

    virtual void set_segment_bold(bool b) = 0;
    virtual void set_segment_italic(bool b) = 0;
    virtual void set_segment_font_name(const char* s, size_t n) = 0;
    virtual void set_segment_font_size(double point) = 0;
    virtual void append_segment(const char* s, size_t n) = 0;
    virtual size_t commit_segments() = 0;
};

/**
 * Interface for styles.
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
    virtual size_t commit_font() = 0;

    // fill

    virtual void set_fill_count(size_t n) = 0;
    virtual void set_fill_pattern_type(const char* s, size_t n) = 0;
    virtual void set_fill_fg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual void set_fill_bg_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue) = 0;
    virtual size_t commit_fill() = 0;

    // border

    virtual void set_border_count(size_t n) = 0;
    virtual void set_border_style(orcus::spreadsheet::border_direction_t dir, const char* s, size_t n) = 0;
    virtual size_t commit_border() = 0;

    // cell protection
    virtual void set_cell_hidden(bool b) = 0;
    virtual void set_cell_locked(bool b) = 0;
    virtual size_t commit_cell_protection() = 0;

    // number format
    virtual void set_number_format(const char* s, size_t n) = 0;
    virtual size_t commit_number_format() = 0;

    // cell style xf

    virtual void set_cell_style_xf_count(size_t n) = 0;
    virtual size_t commit_cell_style_xf() = 0;

    // cell xf

    virtual void set_cell_xf_count(size_t n) = 0;
    virtual size_t commit_cell_xf() = 0;

    // xf (cell format) - used both by cell xf and cell style xf.

    virtual void set_xf_font(size_t index) = 0;
    virtual void set_xf_fill(size_t index) = 0;
    virtual void set_xf_border(size_t index) = 0;
    virtual void set_xf_protection(size_t index) = 0;
    virtual void set_xf_number_format(size_t index) = 0;
    virtual void set_xf_style_xf(size_t index) = 0;

    // cell style entry

    virtual void set_cell_style_count(size_t n) = 0;
    virtual void set_cell_style_name(const char* s, size_t n) = 0;
    virtual void set_cell_style_xf(size_t index) = 0;
    virtual void set_cell_style_builtin(size_t index) = 0;
    virtual size_t commit_cell_style() = 0;
};

/**
 * Interface for sheet.
 */
class import_sheet
{
public:
    ORCUS_DLLPUBLIC virtual ~import_sheet() = 0;

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
     * Set cell format to specified cell.  The cell format is referred to by
     * the xf (cell format) index in the styles table.
     *
     * @param row row ID
     * @param col column ID
     * @param index 0-based xf (cell format) index
     */
    virtual void set_format(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, size_t xf_index) = 0;

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
class import_factory
{
public:
    ORCUS_DLLPUBLIC virtual ~import_factory() = 0;

    /**
     * @return pointer to the shared strings instance. It may return NULL if
     *         the client app doesn't support shared strings.
     */
    virtual import_shared_strings* get_shared_strings() = 0;

    /**
     * @return pointer to the styles instance. It may return NULL if the
     *         client app doesn't support styles.
     */
    virtual import_styles* get_styles() = 0;

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
     * This method is called at the end of import, to give the implementor a
     * chance to perform post-processing if necessary.
     */
    virtual void finalize() = 0;
};

}}}

#endif
