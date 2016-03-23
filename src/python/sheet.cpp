/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sheet.hpp"

#include "orcus/spreadsheet/types.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/document.hpp"

#include <structmember.h>

namespace orcus { namespace python {

sheet_data::~sheet_data()
{
}

namespace {

struct free_pyobj
{
    void operator() (PyObject* p)
    {
        Py_XDECREF(p);
    }
};

/**
 * Python object for orcus.Sheet.
 */
struct sheet
{
    PyObject_HEAD

    PyObject* name;

    sheet_data* m_data;
};

void sheet_dealloc(sheet* self)
{
    delete self->m_data;

    Py_XDECREF(self->name);

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

PyMethodDef sheet_methods[] =
{
    { nullptr }
};

PyMemberDef sheet_members[] =
{
    { (char*)"name", T_OBJECT_EX, offsetof(sheet, name), READONLY, (char*)"sheet name" },
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
    spreadsheet::sheet_t sid = orcus_sheet->get_index();
    pstring name = doc.get_sheet_name(sid);
    pysheet->name = PyUnicode_FromStringAndSize(name.get(), name.size());
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
