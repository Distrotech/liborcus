/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/env.hpp"
#include "orcus/info.hpp"

#include "root.hpp"
#include "xlsx.hpp"
#include "document.hpp"

#include <iostream>
#include <string>

#include <Python.h>

#define ORCUS_DEBUG_PYTHON 0
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

using namespace std;

namespace orcus { namespace python {

namespace {

#if ORCUS_DEBUG_PYTHON
void print_args(PyObject* args)
{
    string args_str;
    PyObject* repr = PyObject_Repr(args);
    if (repr)
    {
        Py_INCREF(repr);
        args_str = PyBytes_AsString(repr);
        Py_DECREF(repr);
    }
    cout << args_str << "\n";
}
#endif

PyMethodDef orcus_methods[] =
{
    { "info", (PyCFunction)info, METH_NOARGS, "Print orcus module information." },

    { "_xlsx_read_file", (PyCFunction)xlsx_read_file, METH_VARARGS | METH_KEYWORDS,
      "Load specified xlsx file into a document model." },

    { nullptr, nullptr, 0, nullptr }
};

struct module_state
{
    PyObject* error;
};

int orcus_traverse(PyObject* m, visitproc visit, void* arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

int orcus_clear(PyObject* m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

}

struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    "_orcus",
    nullptr,
    sizeof(struct module_state),
    orcus_methods,
    nullptr,
    orcus_traverse,
    orcus_clear,
    nullptr
};

}}

extern "C" {

ORCUS_DLLPUBLIC PyObject* PyInit__orcus()
{
    PyTypeObject* doc_type = orcus::python::get_document_type();
    if (PyType_Ready(doc_type) < 0)
        return nullptr;

    PyObject* m = PyModule_Create(&orcus::python::moduledef);

    Py_INCREF(doc_type);
    PyModule_AddObject(m, "Document", reinterpret_cast<PyObject*>(doc_type));

    return m;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
