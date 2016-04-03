/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sheet.hpp"
#include "sheet_rows.hpp"

#include "orcus/spreadsheet/types.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/document.hpp"

#include <structmember.h>
#include <iostream>

namespace orcus { namespace python {

sheet_data::~sheet_data()
{
}

namespace {

/**
 * Python object for orcus.Sheet.
 */
struct sheet
{
    PyObject_HEAD

    PyObject* name;
    PyObject* sheet_size;
    PyObject* data_size;

    sheet_data* m_data;
};

void sheet_dealloc(sheet* self)
{
    delete self->m_data;

    Py_CLEAR(self->name);
    Py_CLEAR(self->sheet_size);
    Py_CLEAR(self->data_size);

    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

PyObject* sheet_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    sheet* self = (sheet*)type->tp_alloc(type, 0);
    self->m_data = new sheet_data;
    return reinterpret_cast<PyObject*>(self);
}

int sheet_init(sheet* self, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    return 0;
}

PyObject* sheet_get_rows(PyObject* self, PyObject* args, PyObject* kwargs)
{
    PyTypeObject* sr_type = get_sheet_rows_type();

    PyObject* rows = sr_type->tp_new(sr_type, nullptr, nullptr);
    if (!rows)
        return nullptr;

    sr_type->tp_init(rows, nullptr, nullptr);

    // Populate the sheet rows data.
    store_sheet_rows_data(rows, reinterpret_cast<sheet*>(self)->m_data->m_sheet);

    Py_INCREF(rows);
    return rows;
}

PyMethodDef sheet_methods[] =
{
    { "get_rows", (PyCFunction)sheet_get_rows, METH_VARARGS, "Get a sheet row iterator." },
    { nullptr }
};

PyMemberDef sheet_members[] =
{
    { (char*)"name",       T_OBJECT_EX, offsetof(sheet, name),       READONLY, (char*)"sheet name" },
    { (char*)"sheet_size", T_OBJECT_EX, offsetof(sheet, sheet_size), READONLY, (char*)"sheet size" },
    { (char*)"data_size",  T_OBJECT_EX, offsetof(sheet, data_size),  READONLY, (char*)"data size" },
    { nullptr }
};

PyTypeObject sheet_type =
{
    PyVarObject_HEAD_INIT(nullptr, 0)
    "orcus.Sheet",                            // tp_name
    sizeof(sheet),                            // tp_basicsize
    0,                                        // tp_itemsize
    (destructor)sheet_dealloc,                // tp_dealloc
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
    "orcus sheet object",                     // tp_doc
    0,		                                  // tp_traverse
    0,		                                  // tp_clear
    0,		                                  // tp_richcompare
    0,		                                  // tp_weaklistoffset
    0,		                                  // tp_iter
    0,		                                  // tp_iternext
    sheet_methods,                            // tp_methods
    sheet_members,                            // tp_members
    0,                                        // tp_getset
    0,                                        // tp_base
    0,                                        // tp_dict
    0,                                        // tp_descr_get
    0,                                        // tp_descr_set
    0,                                        // tp_dictoffset
    (initproc)sheet_init,                     // tp_init
    0,                                        // tp_alloc
    sheet_new,                                // tp_new
};

}

sheet_data* get_sheet_data(PyObject* self)
{
    return reinterpret_cast<sheet*>(self)->m_data;
}

PyTypeObject* get_sheet_type()
{
    return &sheet_type;
}

void store_sheet(
    PyObject* self, const spreadsheet::document& doc, spreadsheet::sheet* orcus_sheet)
{
    sheet* pysheet = reinterpret_cast<sheet*>(self);
    pysheet->m_data->m_sheet = orcus_sheet;

    // Populate the python members.

    // Sheet name
    spreadsheet::sheet_t sid = orcus_sheet->get_index();
    pstring name = doc.get_sheet_name(sid);
    pysheet->name = PyUnicode_FromStringAndSize(name.get(), name.size());

    // Data size - size of the data area.
    ixion::abs_range_t range = orcus_sheet->get_data_range();
    if (range.valid())
    {
        pysheet->data_size = PyDict_New();
        PyDict_SetItemString(pysheet->data_size, "column", PyLong_FromLong(range.last.column+1));
        PyDict_SetItemString(pysheet->data_size, "row", PyLong_FromLong(range.last.row+1));
    }
    else
    {
        Py_INCREF(Py_None);
        pysheet->data_size = Py_None;
    }

    // Sheet size - size of the entire sheet.
    pysheet->sheet_size = PyDict_New();
    PyDict_SetItemString(pysheet->sheet_size, "column", PyLong_FromLong(orcus_sheet->col_size()));
    PyDict_SetItemString(pysheet->sheet_size, "row", PyLong_FromLong(orcus_sheet->row_size()));
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
