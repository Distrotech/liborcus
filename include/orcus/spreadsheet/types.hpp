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

namespace orcus { namespace spreadsheet {

typedef int row_t;
typedef int col_t;
typedef int sheet_t;
typedef unsigned char color_elem_t;
typedef unsigned short col_width_t;
typedef unsigned short row_height_t;

ORCUS_DLLPUBLIC col_width_t get_default_column_width();
ORCUS_DLLPUBLIC row_height_t get_default_row_height();

enum border_direction_t
{
    border_direction_unknown = 0,
    border_top,
    border_bottom,
    border_left,
    border_right,
    border_diagonal
};

enum formula_grammar_t
{
    formula_grammar_unknown = 0,
    formula_grammar_xlsx_2007,
    formula_grammar_xlsx_2010,
    formula_grammar_ods,
    formula_grammar_gnumeric
};

enum formula_t
{
    formula_array,
    formula_data_table,
    formula_normal,
    formula_shared
};

enum underline_t
{
    underline_none,
    underline_single,
    underline_single_accounting, // unique to xlsx
    underline_double,
    underline_double_accounting // unique to xlsx
};

enum hor_alignment_t
{
    hor_alignment_unknown = 0,
    hor_alignment_left,
    hor_alignment_center,
    hor_alignment_right,
    hor_alignment_justified,
    hor_alignment_distributed,
    hor_alignment_filled
};

enum ver_alignment_t
{
    ver_alignment_unknown = 0,
    ver_alignment_top,
    ver_alignment_middle,
    ver_alignment_bottom,
    ver_alignment_justified,
    ver_alignment_distributed
};

/**
 * Type of data table.  A data table can be either of a single-variable
 * column, a single-variable row, or a double-variable type that uses both
 * column and row input cells.
 */
enum data_table_type_t
{
    data_table_column,
    data_table_row,
    data_table_both
};

/**
 * Function type used in the totals row of a table.
 */
enum totals_row_function_t
{
    totals_row_function_none = 0,
    totals_row_function_sum,
    totals_row_function_minimum,
    totals_row_function_maximum,
    totals_row_function_average,
    totals_row_function_count,
    totals_row_function_count_numbers,
    totals_row_function_standard_deviation,
    totals_row_function_variance,
    totals_row_function_custom
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
