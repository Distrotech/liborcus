/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_TYPES_HPP
#define ORCUS_SPREADSHEET_TYPES_HPP

#include "orcus/env.hpp"
#include <cstdlib>
#include <cstdint>

// NB: This header should only use primitive data types and enums.

namespace orcus { namespace spreadsheet {

typedef int32_t row_t;
typedef int32_t col_t;
typedef int32_t sheet_t;
typedef uint8_t color_elem_t;
typedef uint16_t col_width_t;
typedef uint16_t row_height_t;

ORCUS_DLLPUBLIC col_width_t get_default_column_width();
ORCUS_DLLPUBLIC row_height_t get_default_row_height();

enum class border_direction_t
{
    unknown = 0,
    top,
    bottom,
    left,
    right,
    diagonal,
    diagonal_bl_tr,
    diagonal_tl_br
};

enum class border_style_t
{
    unknown = 0,
    none,
    solid,
    dash_dot,
    dash_dot_dot,
    dashed,
    dotted,
    double_border,
    hair,
    medium,
    medium_dash_dot,
    medium_dash_dot_dot,
    medium_dashed,
    slant_dash_dot,
    thick,
    thin
};

enum class formula_grammar_t
{
    unknown = 0,
    xlsx_2007,
    xlsx_2010,
    ods,
    gnumeric
};

enum class formula_t
{
    array,
    data_table,
    normal,
    shared
};

enum class underline_t
{
    none = 0,
    single_line,
    single_accounting, // unique to xlsx
    double_line,
    double_accounting, // unique to xlsx
    solid,
    dotted,
    dash,
    long_dash,
    dot_dash,
    dot_dot_dot_dash,
    wave
};

enum class underline_width_t
{
    none = 0,
    normal,
    bold,
    thin,
    medium,
    thick,
    positive_integer,
    percent,
    positive_length
};

enum class underline_mode_t
{
    continuos = 0,
    skip_white_space
};

enum class underline_type_t
{
    none = 0,
    single,
    double_type            //necessary to not call it "double", since it is a reserved word
};

struct underline_attrs_t
{
    underline_t underline_style;
    underline_width_t underline_width;
    underline_mode_t underline_mode;
    underline_type_t underline_type;
};

enum class hor_alignment_t
{
    unknown = 0,
    left,
    center,
    right,
    justified,
    distributed,
    filled
};

enum class ver_alignment_t
{
    unknown = 0,
    top,
    middle,
    bottom,
    justified,
    distributed
};

/**
 * Type of data table.  A data table can be either of a single-variable
 * column, a single-variable row, or a double-variable type that uses both
 * column and row input cells.
 */
enum class data_table_type_t
{
    column,
    row,
    both
};

/**
 * Function type used in the totals row of a table.
 */
enum class totals_row_function_t
{
    none = 0,
    sum,
    minimum,
    maximum,
    average,
    count,
    count_numbers,
    standard_deviation,
    variance,
    custom
};

enum class conditional_format_t
{
    unknown = 0,
    condition,
    date,
    formula,
    colorscale,
    databar,
    iconset
};

enum class condition_operator_t
{
    unknown = 0,
    equal,
    less,
    greater,
    greater_equal,
    less_equal,
    not_equal,
    between,
    not_between,
    duplicate,
    unique,
    top_n,
    bottom_n,
    above_average,
    below_average,
    above_equal_average,
    below_equal_average,
    contains_error,
    contains_no_error,
    begins_with,
    ends_with,
    contains,
    contains_blanks,
    not_contains,
    expression
};

enum class condition_type_t
{
    unknown = 0,
    value,
    automatic,
    max,
    min,
    formula,
    percent,
    percentile
};

enum class condition_date_t
{
    unknown = 0,
    today,
    yesterday,
    tomorrow,
    last_7_days,
    this_week,
    next_week,
    last_week,
    this_month,
    next_month,
    last_month,
    this_year,
    next_year,
    last_year,
};

enum class databar_axis_t
{
    none = 0,
    middle,
    automatic
};

/**
 * Convert a string representation of a totals row function name to its
 * equivalent enum value.
 */
ORCUS_DLLPUBLIC totals_row_function_t to_totals_row_function_enum(const char* p, size_t n);

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
