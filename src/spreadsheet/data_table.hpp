/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_DATA_TABLE_HPP
#define ORCUS_SPREADSHEET_DATA_TABLE_HPP

#include "orcus/spreadsheet/import_interface.hpp"

namespace orcus { namespace spreadsheet {

class sheet;
struct data_table_impl;

class data_table : public iface::import_data_table
{
    data_table_impl* mp_impl;

public:
    data_table(sheet& sh);
    ~data_table();

    void reset();

    virtual void set_type(data_table_type_t type);

    virtual void set_range(const char* p_range, size_t n_range);

    virtual void set_first_reference(const char* p_ref, size_t n_ref, bool deleted);

    virtual void set_second_reference(const char* p_ref, size_t n_ref, bool deleted);

    virtual void commit();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
