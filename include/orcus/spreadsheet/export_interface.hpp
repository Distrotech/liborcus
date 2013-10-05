/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SPREADSHEET_EXPORT_INTERFACE_HPP__
#define __ORCUS_SPREADSHEET_EXPORT_INTERFACE_HPP__

#include "types.hpp"
#include "../env.hpp"

#include <ostream>

namespace orcus { namespace spreadsheet { namespace iface {

class export_sheet
{
public:
    ORCUS_DLLPUBLIC virtual ~export_sheet() = 0;

    virtual void write_string(std::ostream& os, orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col) const = 0;
};

class export_factory
{
public:
    ORCUS_DLLPUBLIC virtual ~export_factory() = 0;

    virtual const export_sheet* get_sheet(const char* sheet_name, size_t sheet_name_length) const = 0;
};

}}}




#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
