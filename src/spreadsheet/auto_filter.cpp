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

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
