/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/types.hpp"

#include <limits>

namespace orcus { namespace spreadsheet {

const col_width_t default_column_width = std::numeric_limits<col_width_t>::max();
const row_height_t default_row_height = std::numeric_limits<row_height_t>::max();

data_table_t::data_table_t() :
    type(data_table_column),
    range(NULL), range_length(0),
    ref1(NULL), ref1_length(0),
    ref2(NULL), ref2_length(0),
    ref1_deleted(false), ref2_deleted(false) {}

auto_filter_t::auto_filter_t() :
    range(NULL), range_length(0) {}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
