/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx.hpp"

#if defined(__ORCUS_XLSX) && defined(__ORCUS_SPREADSHEET_MODEL)
#include "orcus/orcus_xlsx.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/factory.hpp"
#define XLSX_ENABLED 1
#else
#define XLSX_ENABLED 0
#endif

#include <iostream>

using namespace std;

namespace orcus { namespace python {

#if XLSX_ENABLED

PyObject* xlsx_read_file(PyObject* /*module*/, PyObject* args, PyObject* kwargs)
{
    static const char* kwlist[] = { "filepath", nullptr };

    const char* filepath = nullptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", const_cast<char**>(kwlist), &filepath))
        return nullptr;

    spreadsheet::document doc;
    spreadsheet::import_factory fact(doc);
    orcus_xlsx app(&fact);

    app.read_file(filepath);

    // TODO : return a python document object.

    Py_INCREF(Py_None);
    return Py_None;
}

#else

PyObject* xlsx_read_file(PyObject*, PyObject*, PyObject*)
{
    // TODO : raise a python exception here.
    Py_INCREF(Py_None);
    return Py_None;
}

#endif

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
