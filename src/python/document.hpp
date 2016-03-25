/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PYTHON_DOCUMENT_HPP
#define INCLUDED_ORCUS_PYTHON_DOCUMENT_HPP

#include "orcus/spreadsheet/document.hpp"

#include <memory>
#include <Python.h>

namespace orcus { namespace python {

/** non-python part of the document object. */
struct document_data
{
    std::unique_ptr<spreadsheet::document> m_doc;

    ~document_data();
};

document_data* get_document_data(PyObject* self);

void store_document(PyObject* self, std::unique_ptr<spreadsheet::document>&& doc);

PyTypeObject* get_document_type();

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
