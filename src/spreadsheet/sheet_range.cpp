/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/sheet_range.hpp"
#include "orcus/global.hpp"
#include "orcus/exception.hpp"

#include <mdds/multi_type_vector/collection.hpp>

namespace orcus { namespace spreadsheet {

struct sheet_range::impl
{
    mdds::mtv::collection<ixion::column_store_t> m_columns;

    impl() {}
    impl(const impl& other) {}
    ~impl() {}
};

sheet_range::sheet_range() : mp_impl(orcus::make_unique<impl>()) {}

sheet_range::sheet_range(
    const ixion::column_stores_t& stores,
    row_t row_start, col_t col_start, row_t row_end, col_t col_end) :
    mp_impl(orcus::make_unique<impl>())
{
    mdds::mtv::collection<ixion::column_store_t> cols(stores.begin(), stores.end());
    cols.set_element_range(row_start, row_end-row_start+1);
    cols.set_collection_range(col_start, col_end-col_start+1);
    mp_impl->m_columns.swap(cols);
}

sheet_range::sheet_range(const sheet_range& other) :
    mp_impl(orcus::make_unique<impl>(*other.mp_impl)) {}

sheet_range::sheet_range(sheet_range&& other) :
    mp_impl(std::move(other.mp_impl)) {}

sheet_range::~sheet_range() {}

sheet_range& sheet_range::operator= (sheet_range other)
{
    swap(other);
    return *this;
}

void sheet_range::swap(sheet_range& other)
{
    std::swap(mp_impl, other.mp_impl);
}

sheet_range::const_row_iterator sheet_range::row_begin() const
{
    return mp_impl->m_columns.begin();
}

sheet_range::const_row_iterator sheet_range::row_end() const
{
    return mp_impl->m_columns.end();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
