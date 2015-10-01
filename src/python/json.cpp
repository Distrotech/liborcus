/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/env.hpp"
#include "orcus/json_document_tree.hpp"
#include "orcus/config.hpp"
#include "orcus/pstring.hpp"

#include <algorithm>

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

PyObject* to_pyobject(const json_document_tree::node& node)
{
    switch (node.type())
    {
        case json_node_t::object:
        {
            vector<pstring> keys = node.keys();
            PyObject* o = PyDict_New();

            std::for_each(keys.begin(), keys.end(),
                [&](const pstring& key)
                {
                    PyObject* k = PyUnicode_FromStringAndSize(key.get(), key.size());
                    PyObject* v = to_pyobject(node.child(key));
                    PyDict_SetItem(o, k, v);
                }
            );
            return o;
        }
        case json_node_t::array:
        {
            size_t n = node.child_count();
            PyObject* o = PyList_New(node.child_count());
            for (size_t i = 0; i < n; ++i)
            {
                json_document_tree::node child = node.child(i);
                PyObject* item = to_pyobject(child);
                if (!item)
                    return nullptr;

                PyList_SetItem(o, i, item);
            }
            return o;
        }
        case json_node_t::number:
            return PyFloat_FromDouble(node.numeric_value());
        case json_node_t::string:
        {
            pstring sv = node.string_value();
            return PyUnicode_FromStringAndSize(sv.get(), sv.size());
        }
        case json_node_t::boolean_true:
            Py_INCREF(Py_True);
            return Py_True;
        case json_node_t::boolean_false:
            Py_INCREF(Py_False);
            return Py_False;
        case json_node_t::null:
            Py_INCREF(Py_None);
            return Py_None;
        default:
            ;
    }

    PyErr_SetString(PyExc_TypeError, "Unexpected JSON node type.");
    return nullptr;
}

PyObject* json_loads(PyObject* /*module*/, PyObject* args, PyObject* kwargs)
{
    char* stream = NULL;
    static const char* kwlist[] = { "s", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", const_cast<char**>(kwlist), &stream))
    {
        PyErr_SetString(PyExc_TypeError, "The method must be given a string.");
        return nullptr;
    }

    json_config conf;
    json_document_tree doc;
    doc.load(stream, conf);

    return to_pyobject(doc.get_document_root());
}

PyMethodDef orcus_methods[] =
{
    { "loads", (PyCFunction)json_loads, METH_VARARGS | METH_KEYWORDS, "Load JSON string into a Python object." },
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
