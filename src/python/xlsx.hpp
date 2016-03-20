/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PYTHON_XLSX_HPP
#define INCLUDED_ORCUS_PYTHON_XLSX_HPP

#include <Python.h>

namespace orcus { namespace python {

PyObject* xlsx_read_file(PyObject* module, PyObject* args, PyObject* kwargs);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
