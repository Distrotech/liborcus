/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/types.hpp"
#include "sorted_string_map.hpp"

#include <limits>

namespace orcus { namespace spreadsheet {

namespace {

typedef orcus::sorted_string_map<totals_row_function_t> trf_map_type;

// Keys must be sorted.
trf_map_type::entry trf_entries[] =
{
    { "average", totals_row_function_average },
    { "count", totals_row_function_count },
    { "countNums", totals_row_function_count_numbers },
    { "custom", totals_row_function_custom },
    { "max", totals_row_function_maximum },
    { "min", totals_row_function_minimum },
    { "none", totals_row_function_none },
    { "stdDev", totals_row_function_standard_deviation },
    { "sum", totals_row_function_sum },
    { "var", totals_row_function_variance },
};

const trf_map_type& get_trf_map()
{
    static trf_map_type trf_map(
        trf_entries,
        sizeof(trf_entries)/sizeof(trf_entries[0]),
        totals_row_function_none);

    return trf_map;
}

}

col_width_t get_default_column_width()
{
    return std::numeric_limits<col_width_t>::max();
}

row_height_t get_default_row_height()
{
    return std::numeric_limits<row_height_t>::max();
}

totals_row_function_t to_totals_row_function_enum(const char* p, size_t n)
{
    return get_trf_map().find(p, n);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
