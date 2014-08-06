/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "data_table.hpp"

namespace orcus { namespace spreadsheet {

struct data_table_impl
{
    sheet& m_sheet;

    data_table_impl(sheet& sh) : m_sheet(sh) {}
};

data_table::data_table(sheet& sh) : mp_impl(new data_table_impl(sh)) {}
data_table::~data_table()
{
    delete mp_impl;
}

void data_table::reset()
{
}

void data_table::set_type(orcus::spreadsheet::data_table_type_t type)
{
}

void data_table::set_range(const char* p_range, size_t n_range)
{
}

void data_table::set_first_reference(const char* p_ref, size_t n_ref, bool deleted)
{
}

void data_table::set_second_reference(const char* p_ref, size_t n_ref, bool deleted)
{
}

void data_table::commit()
{
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
