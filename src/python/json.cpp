/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/env.hpp"

#include <iostream>

#include <Python.h>

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

using namespace std;

namespace orcus { namespace python {

namespace {

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

PyObject* test(PyObject*, PyObject*)
{
    cout << "test" << endl;

    Py_INCREF(Py_None);
    return Py_None;
}

PyMethodDef orcus_methods[] =
{
    { "test", (PyCFunction)test, METH_NOARGS, "Test." },
    { NULL, NULL, 0, NULL }
};

struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    "_orcus_json",
    NULL,
    sizeof(struct module_state),
    orcus_methods,
    NULL,
    orcus_traverse,
    orcus_clear,
    NULL
};

}

}}

extern "C" {

ORCUS_DLLPUBLIC PyObject* PyInit__orcus_json()
{
    PyObject* m = PyModule_Create(&orcus::python::moduledef);
    return m;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
