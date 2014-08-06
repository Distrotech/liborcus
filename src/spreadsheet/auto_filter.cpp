/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/auto_filter.hpp"

namespace orcus { namespace spreadsheet {

void auto_filter_column_t::reset()
{
    match_values.clear();
}

auto_filter_t::auto_filter_t() : range(ixion::abs_range_t::invalid) {}

void auto_filter_t::reset()
{
    range = ixion::abs_range_t(ixion::abs_range_t::invalid);
    columns.clear();
}

table_column_t::table_column_t() : identifier(0), totals_row_function(totals_row_function_none) {}

void table_column_t::reset()
{
    identifier = 0;
    name.clear();
    totals_row_label.clear();
    totals_row_function = totals_row_function_none;
}

table_style_t::table_style_t() :
    show_first_column(false),
    show_last_column(false),
    show_row_stripes(false),
    show_column_stripes(false) {}

void table_style_t::reset()
{
    name.clear();
    show_first_column = false;
    show_last_column = false;
    show_row_stripes = false;
    show_column_stripes = false;
}

table_t::table_t() : identifier(0), range(ixion::abs_range_t::invalid), totals_row_count(0) {}

void table_t::reset()
{
    identifier = 0;
    name.clear();
    display_name.clear();
    range = ixion::abs_range_t(ixion::abs_range_t::invalid);
    totals_row_count = 0;
    filter.reset();
    columns.clear();
    style.reset();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
