/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_TYPES_HPP
#define ORCUS_SPREADSHEET_TYPES_HPP

#include "../env.hpp"
#include <cstdlib>

// NB: This header should only use primitive data types and enums.

namespace orcus { namespace spreadsheet {

typedef int row_t;
typedef int col_t;
typedef int sheet_t;
typedef unsigned char color_elem_t;
typedef unsigned short col_width_t;
typedef unsigned short row_height_t;

ORCUS_DLLPUBLIC col_width_t get_default_column_width();
ORCUS_DLLPUBLIC row_height_t get_default_row_height();

enum class border_direction_t
{
    unknown = 0,
    top,
    bottom,
    left,
    right,
    diagonal
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
    double_accounting // unique to xlsx
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
