/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "root.hpp"

#include "orcus/info.hpp"

#include <iostream>

using namespace std;

namespace orcus { namespace python {

PyObject* info(PyObject*, PyObject*)
{
    cout << "orcus version: "
        << orcus::get_version_major() << '.'
        << orcus::get_version_minor() << '.'
        << orcus::get_version_micro() << endl;

    Py_INCREF(Py_None);
    return Py_None;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

