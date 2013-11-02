/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "mock_spreadsheet.hpp"

#include <cassert>

using namespace orcus::spreadsheet;

namespace orcus { namespace spreadsheet { namespace mock {

//import_factory

import_factory::~import_factory()
{
}

orcus::spreadsheet::iface::import_global_settings* import_factory::get_global_settings()
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_shared_strings* import_factory::get_shared_strings()
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_styles* import_factory::get_styles()
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_sheet* import_factory::append_sheet(const char*, size_t)
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_sheet* import_factory::get_sheet(const char*, size_t)
{
    assert(false);
    return NULL;
}

orcus::spreadsheet::iface::import_sheet* import_factory::get_sheet(orcus::spreadsheet::sheet_t)
{
    assert(false);
    return NULL;
}

void import_factory::finalize() {}

// import_shared_strings

import_shared_strings::~import_shared_strings()
{
}

size_t import_shared_strings::append(const char*, size_t)
{
    assert(false);
    return 0;
}

size_t import_shared_strings::add(const char*, size_t)
{
    assert(false);
    return 0;
}

void import_shared_strings::set_segment_font(size_t)
{
    assert(false);
}

void import_shared_strings::set_segment_bold(bool)
{
    assert(false);
}

void import_shared_strings::set_segment_italic(bool)
{
    assert(false);
}

void import_shared_strings::set_segment_font_name(const char*, size_t)
{
    assert(false);
}

void import_shared_strings::set_segment_font_size(double)
{
    assert(false);
}

void import_shared_strings::set_segment_font_color(color_elem_t, color_elem_t, color_elem_t, color_elem_t)
{
    assert(false);
}

void import_shared_strings::append_segment(const char*, size_t)
{
    assert(false);
}

size_t import_shared_strings::commit_segments()
{
    assert(false);
    return 0;
}

// import sheet properties

import_sheet_properties::~import_sheet_properties()
{
}

void import_sheet_properties::set_column_width(col_t, double, length_unit_t)
{
    assert(false);
}

void import_sheet_properties::set_column_hidden(col_t, bool)
{
    assert(false);
}

void import_sheet_properties::set_row_height(row_t, double, length_unit_t)
{
    assert(false);
}

void import_sheet_properties::set_row_hidden(row_t, bool)
{
    assert(false);
}

void import_sheet_properties::set_merge_cell_range(const char*, size_t)
{
    assert(false);
}

// import_sheet

import_sheet::~import_sheet()
{
}

void import_sheet::set_auto(row_t, col_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_value(row_t, col_t, double)
{
    assert(false);
}

void import_sheet::set_bool(row_t, col_t, bool)
{
    assert(false);
}

void import_sheet::set_date_time(row_t, col_t, int, int, int, int, int, double)
{
    assert(false);
}

void import_sheet::set_string(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_format(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_formula(row_t, col_t, formula_grammar_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, formula_grammar_t, size_t,
        const char*, size_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, formula_grammar_t, size_t,
        const char*, size_t)
{
    assert(false);
}

void import_sheet::set_shared_formula(row_t, col_t, size_t)
{
    assert(false);
}

void import_sheet::set_array_formula(
    row_t, col_t, formula_grammar_t, const char*, size_t, row_t, col_t)
{
    assert(false);
}

void import_sheet::set_array_formula(
    row_t, col_t, formula_grammar_t, const char*, size_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_formula_result(row_t, col_t, const char*, size_t)
{
    assert(false);
}

void import_sheet::set_data_table(const orcus::spreadsheet::data_table_t&)
{
    assert(false);
}

}}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
