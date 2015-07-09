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

enum conditional_format_t
{
    conditional_format_unknown = 0,
    conditional_format_condition,
    conditional_format_date,
    conditional_format_formula,
    conditional_format_colorscale,
    conditional_format_databar,
    conditional_format_iconset
};

enum condition_operator_t
{
    condition_operator_unknown = 0,
    condition_operator_equal,
    condition_operator_less,
    condition_operator_greater,
    condition_operator_greater_equal,
    condition_operator_less_equal,
    condition_operator_not_equal,
    condition_operator_between,
    condition_operator_not_between,
    condition_operator_duplicate,
    condition_operator_unique,
    condition_operator_top_n,
    condition_operator_bottom_n,
    condition_operator_above_average,
    condition_operator_below_average,
    condition_operator_above_equal_average,
    condition_operator_below_equal_average,
    condition_operator_contains_error,
    condition_operator_contains_no_error,
    condition_operator_begins_with,
    condition_operator_ends_with,
    condition_operator_contains,
    condition_operator_contains_blanks,
    condition_operator_not_contains,
    condition_operator_expression
};

enum condition_type_t
{
    condition_type_unknown = 0,
    condition_type_value,
    condition_type_auto,
    condition_type_max,
    condition_type_min,
    condition_type_formula,
    condition_type_percent,
    condition_type_percentile
};

enum condition_date_t
{
    condition_date_unknown = 0,
    condition_date_today,
    condition_date_yesterday,
    condition_date_tomorrow,
    condition_date_last_7_days,
    condition_date_this_week,
    condition_date_next_week,
    condition_date_last_week,
    condition_date_this_month,
    condition_date_next_month,
    condition_date_last_month,
    condition_date_this_year,
    condition_date_next_year,
    condition_date_last_year,
};

enum databar_axis_t
{
    databar_axis_none = 0,
    databar_axis_middle,
    databar_axis_automatic
};

/**
 * Convert a string representation of a totals row function name to its
 * equivalent enum value.
 */
ORCUS_DLLPUBLIC totals_row_function_t to_totals_row_function_enum(const char* p, size_t n);

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
