/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "sheet_rows.hpp"
#include "orcus/spreadsheet/sheet.hpp"

#include <ixion/cell.hpp>
#include <ixion/formula_result.hpp>

namespace orcus { namespace python {

sheet_rows_data::sheet_rows_data() :
    m_sheet(nullptr),
    m_range(ixion::abs_range_t::invalid),
    m_current_row(-1) {}

sheet_rows_data::~sheet_rows_data() {}

namespace {

/**
 * Python object for orcus.SheetRows.
 */
struct pyobj_sheet_rows
{
    PyObject_HEAD

    sheet_rows_data* m_data;
};

void sheet_rows_dealloc(pyobj_sheet_rows* self)
{
    delete self->m_data;

    Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

PyObject* sheet_rows_new(PyTypeObject* type, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    pyobj_sheet_rows* self = (pyobj_sheet_rows*)type->tp_alloc(type, 0);
    self->m_data = new sheet_rows_data;
    return reinterpret_cast<PyObject*>(self);
}

int sheet_rows_init(pyobj_sheet_rows* self, PyObject* /*args*/, PyObject* /*kwargs*/)
{
    return 0;
}

PyObject* sheet_rows_iter(PyObject* self)
{
    sheet_rows_data* data = reinterpret_cast<pyobj_sheet_rows*>(self)->m_data;
    data->m_current_row = 0;
    data->m_row_pos = data->m_sheet_range.row_begin();
    data->m_row_end = data->m_sheet_range.row_end();

    Py_INCREF(self);
    return self;
}

PyObject* sheet_rows_iternext(PyObject* self)
{
    sheet_rows_data* data = reinterpret_cast<pyobj_sheet_rows*>(self)->m_data;
    auto& row_pos = data->m_row_pos;
    const auto& row_end = data->m_row_end;

    if (row_pos == row_end)
    {
        // No more elements.  Stop the iteration.
        PyErr_SetNone(PyExc_StopIteration);
        return nullptr;
    }

    size_t row_position = row_pos->position;
    PyObject* pyobj_row = PyTuple_New(data->m_range.last.column+1);
    for (; row_pos != row_end && row_position == row_pos->position; ++row_pos)
    {
        size_t col_pos = row_pos->index;

        switch (row_pos->type)
        {
            case ixion::element_type_empty:
            {
                Py_INCREF(Py_None);
                PyTuple_SetItem(pyobj_row, col_pos, Py_None);
            }
            break;
            case ixion::element_type_boolean:
                // TODO: This doesn't work currently due to the vector<bool> situation...
            break;
            case ixion::element_type_string:
            {
                ixion::string_id_t sid = row_pos->get<ixion::string_element_block>();
                const std::string* ps = data->m_sheet_range.get_string(sid);
                if (ps)
                {
                    PyTuple_SetItem(
                        pyobj_row, col_pos,
                        PyUnicode_FromStringAndSize(ps->data(), ps->size()));
                }
            }
            break;
            case ixion::element_type_numeric:
                PyTuple_SetItem(
                    pyobj_row, col_pos,
                    PyFloat_FromDouble(row_pos->get<ixion::numeric_element_block>()));
            break;
            case ixion::element_type_formula:
            {
                const ixion::formula_cell* fc = row_pos->get<ixion::formula_element_block>();
                const ixion::formula_result& res = fc->get_result_cache();
                switch (res.get_type())
                {
                    case ixion::formula_result::result_type::value:
                        PyTuple_SetItem(
                            pyobj_row, col_pos,
                            PyFloat_FromDouble(res.get_value()));
                    break;
                    case ixion::formula_result::result_type::string:
                    {
                        ixion::string_id_t sid = res.get_string();
                        const std::string* ps = data->m_sheet_range.get_string(sid);
                        if (ps)
                        {
                            PyTuple_SetItem(
                                pyobj_row, col_pos,
                                PyUnicode_FromStringAndSize(ps->data(), ps->size()));
                        }
                    }
                    break;
                    case ixion::formula_result::result_type::error:
                    {
                        ixion::formula_error_t fe = res.get_error();
                        const char* fename = ixion::get_formula_error_name(fe);
                        if (fename)
                            PyTuple_SetItem(pyobj_row, col_pos, PyUnicode_FromString(fename));
                    }
                    break;
                }
            }
            break;
            default:
                ;
        }
    }

    return pyobj_row;
}

PyTypeObject sheet_rows_type =
{
    PyVarObject_HEAD_INIT(nullptr, 0)
    "orcus.SheetRows",                        // tp_name
    sizeof(pyobj_sheet_rows),                       // tp_basicsize
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
    (getiterfunc)sheet_rows_iter,		      // tp_iter
    (iternextfunc)sheet_rows_iternext,        // tp_iternext
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

void store_sheet_rows_data(PyObject* self, const spreadsheet::sheet* orcus_sheet)
{
    sheet_rows_data* data = reinterpret_cast<pyobj_sheet_rows*>(self)->m_data;
    data->m_sheet = orcus_sheet;
    data->m_range = orcus_sheet->get_data_range();

    const ixion::abs_range_t& range = data->m_range;
    data->m_sheet_range = orcus_sheet->get_sheet_range(
        0, 0, range.last.row, range.last.column);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
