/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "document.hpp"
#include "sheet.hpp"
#include "orcus/pstring.hpp"

#include <structmember.h>

using namespace std;

namespace orcus { namespace python {

document_data::~document_data()
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
 * Python object for orcus.Document.
 */
struct document
{
    PyObject_HEAD

    PyObject* sheets; // tuple of sheet objects.

    document_data* m_data;
};

void document_dealloc(document* self)
{
    delete self->m_data;

    // Destroy all sheet objects.
    Py_ssize_t n = PyTuple_Size(self->sheets);
    for (Py_ssize_t i = 0; i < n; ++i)
    {
        PyObject* o = PyTuple_GetItem(self->sheets, i);
        Py_XDECREF(o);
    }
    Py_XDECREF(self->sheets);  // and the tuple containing the sheets.

    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

PyObject* document_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    document* self = (document*)type->tp_alloc(type, 0);
    self->m_data = new document_data;
    return reinterpret_cast<PyObject*>(self);
}

int document_init(document* self, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    return 0;
}

PyMethodDef document_methods[] =
{
    { nullptr }
};

PyMemberDef document_members[] =
{
    { (char*)"sheets", T_OBJECT_EX, offsetof(document, sheets), READONLY, (char*)"sheet objects" },
    { nullptr }
};

PyTypeObject document_type =
{
    PyVarObject_HEAD_INIT(nullptr, 0)
    "orcus.Document",                         // tp_name
    sizeof(document),                         // tp_basicsize
    0,                                        // tp_itemsize
    (destructor)document_dealloc,             // tp_dealloc
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
    "orcus document object",                  // tp_doc
    0,		                                  // tp_traverse
    0,		                                  // tp_clear
    0,		                                  // tp_richcompare
    0,		                                  // tp_weaklistoffset
    0,		                                  // tp_iter
    0,		                                  // tp_iternext
    document_methods,                         // tp_methods
    document_members,                         // tp_members
    0,                                        // tp_getset
    0,                                        // tp_base
    0,                                        // tp_dict
    0,                                        // tp_descr_get
    0,                                        // tp_descr_set
    0,                                        // tp_dictoffset
    (initproc)document_init,                  // tp_init
    0,                                        // tp_alloc
    document_new,                             // tp_new
};

}

document_data* get_document_data(PyObject* self)
{
    return reinterpret_cast<document*>(self)->m_data;
}

void store_document(PyObject* self, std::unique_ptr<spreadsheet::document>&& doc)
{
    document* pydoc = reinterpret_cast<document*>(self);
    document_data* pydoc_data = pydoc->m_data;
    pydoc_data->m_doc = std::move(doc);

    PyTypeObject* sheet_type = get_sheet_type();
    if (!sheet_type)
        return;

    // TODO : Create a tuple of sheet names and store it with the pydoc instance.
    size_t sheet_size = pydoc_data->m_doc->sheet_size();

    pydoc->sheets = PyTuple_New(sheet_size);

    for (size_t i = 0; i < sheet_size; ++i)
    {
        spreadsheet::sheet* sheet = pydoc_data->m_doc->get_sheet(i);
        if (!sheet)
            continue;

        PyObject* pysheet = sheet_type->tp_new(sheet_type, nullptr, nullptr);
        if (!pysheet)
            continue;

        sheet_type->tp_init(pysheet, nullptr, nullptr);

        Py_INCREF(pysheet);
        PyTuple_SetItem(pydoc->sheets, i, pysheet);

        store_sheet(pysheet, *pydoc_data->m_doc, sheet);
    }
}

PyTypeObject* get_document_type()
{
    return &document_type;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
