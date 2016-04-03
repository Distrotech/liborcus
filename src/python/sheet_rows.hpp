/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PYTHON_SHEET_ROWS_HPP
#define INCLUDED_ORCUS_PYTHON_SHEET_ROWS_HPP

#include "orcus/spreadsheet/sheet_range.hpp"

#include <Python.h>
#include <ixion/address.hpp>

namespace orcus {

namespace spreadsheet {

class sheet;
class document;

}

namespace python {

/** non-python part. */
struct sheet_rows_data
{
    const spreadsheet::sheet* m_sheet;
    spreadsheet::sheet_range m_sheet_range;
    ixion::abs_range_t m_range;

    ixion::row_t m_current_row;

    spreadsheet::sheet_range::const_row_iterator m_row_pos;
    spreadsheet::sheet_range::const_row_iterator m_row_end;

    sheet_rows_data();
    ~sheet_rows_data();
};

PyTypeObject* get_sheet_rows_type();

void store_sheet_rows_data(PyObject* self, const spreadsheet::sheet* orcus_sheet);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
