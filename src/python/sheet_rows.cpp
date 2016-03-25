/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sheet_rows.hpp"

namespace orcus { namespace python {

sheet_rows_data::~sheet_rows_data() {}

namespace {

/**
 * Python object for orcus.SheetRows.
 */
struct sheet_rows
{
    PyObject_HEAD

    sheet_rows_data* m_data;
};

void sheet_rows_dealloc(sheet_rows* self)
{
    delete self->m_data;

    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

PyObject* sheet_rows_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    sheet_rows* self = (sheet_rows*)type->tp_alloc(type, 0);
    self->m_data = new sheet_rows_data;
    return reinterpret_cast<PyObject*>(self);
}

int sheet_rows_init(sheet_rows* self, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    return 0;
}

PyTypeObject sheet_rows_type =
{
    PyVarObject_HEAD_INIT(nullptr, 0)
    "orcus.SheetRows",                        // tp_name
    sizeof(sheet_rows),                       // tp_basicsize
    0,                                        // tp_itemsize
    (destructor)sheet_rows_dealloc,           // tp_dealloc
    0,                                        // tp_print
    0,                                        // tp_getattr
    0,                                        // tp_setattr
    0,                                        // tp_compare
    0,                                        // tp_repr
    0,                                        // tp_as_number
    0,                                        // tp_as_sequence
    0,                                        // tp_as_mapping
    0,                                        // tp_hash
    0,                                        // tp_call
    0,                                        // tp_str
    0,                                        // tp_getattro
    0,                                        // tp_setattro
    0,                                        // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
    "orcus sheet rows iterator",              // tp_doc
    0,		                                  // tp_traverse
    0,		                                  // tp_clear
    0,		                                  // tp_richcompare
    0,		                                  // tp_weaklistoffset
    0,		                                  // tp_iter
    0,		                                  // tp_iternext
    0,                                        // tp_methods
    0,                                        // tp_members
    0,                                        // tp_getset
    0,                                        // tp_base
    0,                                        // tp_dict
    0,                                        // tp_descr_get
    0,                                        // tp_descr_set
    0,                                        // tp_dictoffset
    (initproc)sheet_rows_init,                // tp_init
    0,                                        // tp_alloc
    sheet_rows_new,                           // tp_new
};

}

PyTypeObject* get_sheet_rows_type()
{
    return &sheet_rows_type;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
