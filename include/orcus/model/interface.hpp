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

#ifndef __ORCUS_MODEL_INTERFACE_HPP__
#define __ORCUS_MODEL_INTERFACE_HPP__

#include <cstdlib>

#include "types.hpp"

namespace orcus { namespace model { namespace iface {

/**
 * Interface class designed to be derived by the implementor.
 */
class shared_strings
{
public:
    virtual ~shared_strings() = 0;

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

inline shared_strings::~shared_strings() {}

/**
 * Interface for styles.
 */
class styles
{
public:
    virtual ~styles() = 0;

    // font

    virtual void set_font_count(size_t n) = 0;
    virtual void set_font_bold(bool b) = 0;
    virtual void set_font_italic(bool b) = 0;
    virtual void set_font_name(const char* s, size_t n) = 0;
    virtual void set_font_size(double point) = 0;
    virtual void commit_font() = 0;

    // fill

    virtual void set_fill_count(size_t n) = 0;
    virtual void set_fill_pattern_type(const char* s, size_t n) = 0;
    virtual void set_fill_fg_color(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) = 0;
    virtual void set_fill_bg_color(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) = 0;
    virtual void commit_fill() = 0;

    // border

    virtual void set_border_count(size_t n) = 0;
    virtual void set_border_style(orcus::model::border_direction_t dir, const char* s, size_t n) = 0;
    virtual void commit_border() = 0;

    // cell style xf

    virtual void set_cell_style_xf_count(size_t n) = 0;
    virtual void commit_cell_style_xf() = 0;

    // cell xf

    virtual void set_cell_xf_count(size_t n) = 0;
    virtual void commit_cell_xf() = 0;

    // xf (cell format) - used both by cell xf and cell style xf.

    virtual void set_xf_number_format(size_t index) = 0;
    virtual void set_xf_font(size_t index) = 0;
    virtual void set_xf_fill(size_t index) = 0;
    virtual void set_xf_border(size_t index) = 0;
    virtual void set_xf_style_xf(size_t index) = 0;

    // cell style entry

    virtual void set_cell_style_count(size_t n) = 0;
    virtual void set_cell_style_name(const char* s, size_t n) = 0;
    virtual void set_cell_style_xf(size_t index) = 0;
    virtual void set_cell_style_builtin(size_t index) = 0;
    virtual void commit_cell_style() = 0;
};

inline styles::~styles() {}

/**
 * Interface for sheet.
 */
class sheet
{
public:
    virtual ~sheet() = 0;

    /**
     * Set raw string value to a cell and have the implementation
     * auto-recognize its data type.
     *
     * @param row row ID
     * @param col column ID
     * @param p pointer to the first character of the raw string value.
     * @param n size of the raw string value.
     */
    virtual void set_auto(orcus::model::row_t row, orcus::model::col_t col, const char* p, size_t n) = 0;

    /**
     * Set string value to a cell.
     *
     * @param row row ID
     * @param col column ID
     * @param sindex 0-based string index in the shared string table.
     */
    virtual void set_string(orcus::model::row_t row, orcus::model::col_t col, size_t sindex) = 0;

    /**
     * Set numerical value to a cell.
     *
     * @param row row ID
     * @param col column ID
     * @param value value being assigned to the cell.
     */
    virtual void set_value(orcus::model::row_t row, orcus::model::col_t col, double value) = 0;

    /**
     * Set cell format to specified cell.  The cell format is referred to by
     * the xf (cell format) index in the styles table.
     *
     * @param row row ID
     * @param col column ID
     * @param index 0-based xf (cell format) index
     */
    virtual void set_format(orcus::model::row_t row, orcus::model::col_t col, size_t xf_index) = 0;

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
        orcus::model::row_t row, orcus::model::col_t col, orcus::model::formula_grammar_t grammar,
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
     */
    virtual void set_shared_formula(
        orcus::model::row_t row, orcus::model::col_t col, orcus::model::formula_grammar_t grammar,
        size_t sindex, const char* p_formula, size_t n_formula, const char* p_range, size_t n_range) = 0;

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
        orcus::model::row_t row, orcus::model::col_t col, size_t sindex) = 0;

    virtual void set_formula_result(
        orcus::model::row_t row, orcus::model::col_t col, const char* p, size_t n) = 0;
};

inline sheet::~sheet() {}

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
class factory
{
public:
    virtual ~factory() = 0;

    virtual shared_strings* get_shared_strings() = 0;
    virtual styles* get_styles() = 0;
    virtual sheet* append_sheet(const char* sheet_name, size_t sheet_name_length) = 0;
};

inline factory::~factory() {}

}}}

#endif
