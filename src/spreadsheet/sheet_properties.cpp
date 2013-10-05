/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/sheet_properties.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/measurement.hpp"

#include <iostream>

using namespace std;

namespace orcus { namespace spreadsheet {

struct sheet_properties_impl
{
    document& m_doc;
    sheet& m_sheet;

    sheet_properties_impl(document& doc, sheet& sh) : m_doc(doc), m_sheet(sh) {}
};

sheet_properties::sheet_properties(document& doc, sheet& sh) :
    mp_impl(new sheet_properties_impl(doc, sh)) {}

sheet_properties::~sheet_properties()
{
    delete mp_impl;
}

void sheet_properties::set_column_width(col_t col, double width, orcus::length_unit_t unit)
{
    col_width_t w = orcus::convert(width, unit, length_unit_twip);
    mp_impl->m_sheet.set_col_width(col, w);
}

void sheet_properties::set_column_hidden(col_t col, bool hidden)
{
    mp_impl->m_sheet.set_col_hidden(col, hidden);
}

void sheet_properties::set_row_height(row_t row, double height, orcus::length_unit_t unit)
{
    row_height_t h = orcus::convert(height, unit, length_unit_twip);
    mp_impl->m_sheet.set_row_height(row, h);
}

void sheet_properties::set_row_hidden(row_t row, bool hidden)
{
    mp_impl->m_sheet.set_row_hidden(row, hidden);
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
