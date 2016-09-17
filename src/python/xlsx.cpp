/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx.hpp"

#ifdef __ORCUS_PYTHON_XLSX
#include "document.hpp"
#include "orcus/orcus_xlsx.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/factory.hpp"
#include "orcus/global.hpp"
#endif

#include <iostream>

using namespace std;

namespace orcus { namespace python {

#ifdef __ORCUS_PYTHON_XLSX

PyObject* xlsx_read_file(PyObject* /*module*/, PyObject* args, PyObject* kwargs)
{
    static const char* kwlist[] = { "filepath", nullptr };

    const char* filepath = nullptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", const_cast<char**>(kwlist), &filepath))
        return nullptr;

    std::unique_ptr<spreadsheet::document> doc = orcus::make_unique<spreadsheet::document>();
    spreadsheet::import_factory fact(*doc);
    orcus_xlsx app(&fact);

    app.read_file(filepath);

    PyTypeObject* doc_type = get_document_type();
    if (!doc_type)
        return nullptr;

    PyObject* obj_doc = doc_type->tp_new(doc_type, nullptr, nullptr);
    if (!obj_doc)
        return nullptr;

    doc_type->tp_init(obj_doc, nullptr, nullptr);

    store_document(obj_doc, std::move(doc));

    Py_INCREF(obj_doc);
    return obj_doc;
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
