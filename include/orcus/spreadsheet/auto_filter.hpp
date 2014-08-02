/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_AUTO_FILTER_HPP
#define ORCUS_SPREADSHEET_AUTO_FILTER_HPP

#include "types.hpp"
#include "orcus/pstring.hpp"
#include "../env.hpp"

#include <vector>
#include <map>

#include <ixion/address.hpp>

namespace orcus { namespace spreadsheet {

struct ORCUS_DLLPUBLIC auto_filter_column_t
{
    std::vector<pstring> match_values;

    void reset();
};

struct ORCUS_DLLPUBLIC auto_filter_t
{
    typedef std::map<col_t, auto_filter_column_t> columns_type;

    ixion::abs_range_t range;

    columns_type columns;

    auto_filter_t();

    void reset();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
