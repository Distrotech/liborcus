/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SHEET_PROPERTIES_HPP
#define ORCUS_SHEET_PROPERTIES_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"

namespace orcus { namespace spreadsheet {

class document;
class sheet;
struct sheet_properties_impl;

/**
 * Implement the sheet properties import interface, but the actual
 * properties are stored in sheet.
 */
class sheet_properties : public iface::import_sheet_properties
{
    sheet_properties_impl* mp_impl;
public:
    sheet_properties(document& doc, sheet& sh);
    ~sheet_properties();

    virtual void set_column_width(col_t col, double width, orcus::length_unit_t unit);
    virtual void set_column_hidden(col_t col, bool hidden);
    virtual void set_row_height(row_t row, double height, orcus::length_unit_t unit);
    virtual void set_row_hidden(row_t row, bool hidden);
    virtual void set_merge_cell_range(const char* p_ref, size_t p_ref_len);
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
