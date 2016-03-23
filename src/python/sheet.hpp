/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PYTHON_SHEET_HPP
#define INCLUDED_ORCUS_PYTHON_SHEET_HPP

#include <Python.h>

namespace orcus {

namespace spreadsheet {

class sheet;
class document;

}

namespace python {

/** non-python part of the sheet object. */
struct sheet_data
{
    spreadsheet::sheet* m_sheet;

    ~sheet_data();
};

PyTypeObject* get_sheet_type();

void store_sheet(
    PyObject* self, const spreadsheet::document& doc, spreadsheet::sheet* orcus_sheet);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
