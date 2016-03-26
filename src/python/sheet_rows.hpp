/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PYTHON_SHEET_ROWS_HPP
#define INCLUDED_ORCUS_PYTHON_SHEET_ROWS_HPP

#include <Python.h>
#include <ixion/address.hpp>

namespace orcus {

namespace spreadsheet {

class sheet;
class document;

}

namespace python {

struct sheet_rows_data
{
    const spreadsheet::sheet* m_sheet;
    ixion::abs_range_t m_range;
    ixion::row_t m_current_row;

    sheet_rows_data();
    ~sheet_rows_data();
};

PyTypeObject* get_sheet_rows_type();

void store_sheet_rows_data(PyObject* self, const spreadsheet::sheet* orcus_sheet);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
