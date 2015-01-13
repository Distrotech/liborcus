/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/types.hpp"
#include "orcus/global.hpp"

#include <limits>

#include <mdds/sorted_string_map.hpp>

namespace orcus { namespace spreadsheet {

namespace {

typedef mdds::sorted_string_map<totals_row_function_t> trf_map_type;

// Keys must be sorted.
trf_map_type::entry trf_entries[] =
{
    { ORCUS_ASCII("average"), totals_row_function_average },
    { ORCUS_ASCII("count"), totals_row_function_count },
    { ORCUS_ASCII("countNums"), totals_row_function_count_numbers },
    { ORCUS_ASCII("custom"), totals_row_function_custom },
    { ORCUS_ASCII("max"), totals_row_function_maximum },
    { ORCUS_ASCII("min"), totals_row_function_minimum },
    { ORCUS_ASCII("none"), totals_row_function_none },
    { ORCUS_ASCII("stdDev"), totals_row_function_standard_deviation },
    { ORCUS_ASCII("sum"), totals_row_function_sum },
    { ORCUS_ASCII("var"), totals_row_function_variance },
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
