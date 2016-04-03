/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SPREADSHEET_SHEET_RANGE_HPP
#define INCLUDED_ORCUS_SPREADSHEET_SHEET_RANGE_HPP

#include "orcus/env.hpp"
#include "orcus/spreadsheet/types.hpp"
#include <memory>
#include <ixion/column_store_type.hpp>
#include <mdds/multi_type_vector/collection.hpp>

namespace orcus { namespace spreadsheet {

class sheet;

class ORCUS_SPM_DLLPUBLIC sheet_range
{
    typedef mdds::mtv::collection<ixion::column_store_t> columns_type;

    friend class sheet;

    struct impl;
    std::unique_ptr<impl> mp_impl;

    sheet_range(
        const ixion::column_stores_t& stores,
        row_t row_start, col_t col_start, row_t row_end, col_t col_end);

public:
    typedef columns_type::const_iterator const_row_iterator;

    sheet_range();
    sheet_range(const sheet_range& other);
    sheet_range(sheet_range&& other);
    ~sheet_range();

    sheet_range& operator= (sheet_range other);

    void swap(sheet_range& other);

    const_row_iterator row_begin() const;
    const_row_iterator row_end() const;
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
