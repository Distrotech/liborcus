/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "table.hpp"

namespace orcus { namespace spreadsheet {

iface::import_auto_filter* table::get_auto_filter()
{
    return NULL;
}

void table::set_range(const char* p_ref, size_t n_ref) {}
void table::set_identifier(size_t id) {}
void table::set_name(const char* p, size_t n) {}
void table::set_display_name(const char* p, size_t n) {}
void table::set_totals_row_count(size_t row_count) {}

void table::set_column_count(size_t n) {}

void table::set_column_identifier(size_t id) {}
void table::set_column_name(const char* p, size_t n) {}
void table::set_column_totals_row_label(const char* p, size_t n) {}
void table::set_column_totals_row_function(orcus::spreadsheet::totals_row_function_t func) {}
void table::commit_column() {}

void table::commit() {}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
