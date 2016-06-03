/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/env.hpp"
#include "orcus/json_parser.hpp"
#include "orcus/json_document_tree.hpp"
#include "orcus/config.hpp"
#include "orcus/pstring.hpp"

#include <algorithm>
#include <sstream>
#include <boost/current_function.hpp>

#include <Python.h>

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

using namespace std;

namespace orcus { namespace python {

namespace {

class python_json_error : public general_error
{
public:
    python_json_error(const std::string& msg) : general_error("python_json_error", msg) {}
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

struct parser_stack
{
    PyObject* key;
    PyObject* node;
    json_node_t type;

    parser_stack(PyObject* _node, json_node_t _type) : key(nullptr), node(_node), type(_type) {}
};

class json_parser_handler
{
    PyObject* m_root;
    std::vector<parser_stack> m_stack;

    PyObject* push_value(PyObject* value)
    {
        if (!value)
        {
            std::ostringstream os;
            os << BOOST_CURRENT_FUNCTION << ": Empty value is passed.";
            throw python_json_error(os.str());
        }

        if (m_stack.empty())
        {
            std::ostringstream os;
            os << BOOST_CURRENT_FUNCTION << ": Stack is unexpectedly empty.";
            throw python_json_error(os.str());
        }

        parser_stack& cur = m_stack.back();

        switch (cur.type)
        {
            case json_node_t::array:
            {
                PyList_Append(cur.node, value);
                return value;
            }
            break;
            case json_node_t::object:
            {
                assert(cur.key);
                PyDict_SetItem(cur.node, cur.key, value);
                cur.key = nullptr;
                return value;
            }
            break;
            default:
                Py_DECREF(value);
        }

        std::ostringstream os;
        os << BOOST_CURRENT_FUNCTION << ": unstackable JSON value type.";
        throw python_json_error(os.str());
    }

public:
    json_parser_handler() : m_root(nullptr) {}

    ~json_parser_handler()
    {
        if (m_root)
            Py_XDECREF(m_root);

        std::for_each(m_stack.begin(), m_stack.end(),
            [](parser_stack& ps)
            {
                if (ps.key)
                {
                    Py_XDECREF(ps.key);
                    ps.key = nullptr;
                }
            }
        );
    }

    void begin_parse()
    {
        if (m_root)
        {
            std::ostringstream os;
            os << BOOST_CURRENT_FUNCTION << ": Root JSON value already exists.";
            throw python_json_error(os.str());
        }
    }

    void end_parse() {}

    void begin_array()
    {
        if (m_root)
        {
            PyObject* array = push_value(PyList_New(0));
            m_stack.push_back(parser_stack(array, json_node_t::array));
        }
        else
        {
            m_root = PyList_New(0);
            m_stack.push_back(parser_stack(m_root, json_node_t::array));
        }
    }

    void end_array()
    {
        if (m_stack.empty())
        {
            std::ostringstream os;
            os << BOOST_CURRENT_FUNCTION << ": Stack is unexpectedly empty.";
            throw python_json_error(os.str());
        }

        m_stack.pop_back();
    }

    void begin_object()
    {
        if (m_root)
        {
            PyObject* dict = push_value(PyDict_New());
            m_stack.push_back(parser_stack(dict, json_node_t::object));
        }
        else
        {
            m_root = PyDict_New();
            m_stack.push_back(parser_stack(m_root, json_node_t::object));
        }
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        parser_stack& cur = m_stack.back();
        cur.key = PyUnicode_FromStringAndSize(p, len);
    }

    void end_object()
    {
        if (m_stack.empty())
        {
            std::ostringstream os;
            os << BOOST_CURRENT_FUNCTION << ": Stack is unexpectedly empty.";
            throw python_json_error(os.str());
        }

        m_stack.pop_back();
    }

    void boolean_true()
    {
        Py_INCREF(Py_True);
        push_value(Py_True);
    }

    void boolean_false()
    {
        Py_INCREF(Py_False);
        push_value(Py_False);
    }

    void null()
    {
        Py_INCREF(Py_None);
        push_value(Py_None);
    }

    void string(const char* p, size_t len, bool transient)
    {
        push_value(PyUnicode_FromStringAndSize(p, len));
    }

    void number(double val)
    {
        push_value(PyFloat_FromDouble(val));
    }

    PyObject* get_root()
    {
        PyObject* o = m_root;
        m_root = nullptr;
        return o;
    }
};

PyObject* json_loads(PyObject* /*module*/, PyObject* args, PyObject* kwargs)
{
    char* stream = NULL;
    static const char* kwlist[] = { "s", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", const_cast<char**>(kwlist), &stream))
    {
        PyErr_SetString(PyExc_TypeError, "The method must be given a string.");
        return nullptr;
    }

    json_parser_handler hdl;
    orcus::json_parser<json_parser_handler> parser(stream, strlen(stream), hdl);
    try
    {
        parser.parse();
        return hdl.get_root();
    }
    catch (const orcus::json::parse_error& e)
    {
        PyErr_SetString(PyExc_TypeError, e.what());
    }
    return nullptr;
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
