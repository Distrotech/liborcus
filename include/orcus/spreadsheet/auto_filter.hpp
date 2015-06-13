/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SPREADSHEET_AUTO_FILTER_HPP
#define INCLUDED_ORCUS_SPREADSHEET_AUTO_FILTER_HPP

#include "types.hpp"
#include "orcus/pstring.hpp"
#include "../env.hpp"

#include <map>
#include <unordered_set>

#include <ixion/address.hpp>

namespace orcus { namespace spreadsheet {

/**
 * Data for a single column inside autofilter range.
 */
struct ORCUS_SPM_DLLPUBLIC auto_filter_column_t
{
    typedef std::unordered_set<pstring, pstring::hash> match_values_type;
    match_values_type match_values;

    void reset();
    void swap(auto_filter_column_t& r);
};

/**
 * Data for a single autofilter entry.  An autofilter can belong to either a
 * sheet or a table.
 */
struct ORCUS_SPM_DLLPUBLIC auto_filter_t
{
    typedef std::map<col_t, auto_filter_column_t> columns_type;

    ixion::abs_range_t range;

    columns_type columns;

    auto_filter_t();

    void reset();
    void swap(auto_filter_t& r);

    /**
     * Set column data to specified column index.  The data may not contain
     * the original data after the call when overwriting it with an existing
     * data previously associated with the same column index.
     *
     * @param col column index to associate the data to.
     * @param data column data.
     */
    void commit_column(col_t col, auto_filter_column_t& data);
};

/**
 * Single column entry in table.
 */
struct ORCUS_SPM_DLLPUBLIC table_column_t
{
    size_t identifier;
    pstring name;
    pstring totals_row_label;
    totals_row_function_t totals_row_function;

    table_column_t();

    void reset();
};

/**
 * Table style information.
 */
struct ORCUS_SPM_DLLPUBLIC table_style_t
{
    pstring name;

    bool show_first_column:1;
    bool show_last_column:1;
    bool show_row_stripes:1;
    bool show_column_stripes:1;

    table_style_t();

    void reset();
};

/**
 * Single table entry.  A table is a range in a spreadsheet that represents
 * a single set of data that can be used as a data source.
 */
struct ORCUS_SPM_DLLPUBLIC table_t
{
    typedef std::vector<table_column_t> columns_type;

    size_t identifier;

    pstring name;
    pstring display_name;

    ixion::abs_range_t range;

    size_t totals_row_count;

    auto_filter_t filter;
    columns_type columns;
    table_style_t style;

    table_t();

    void reset();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
