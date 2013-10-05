/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XLSX_TYPES_HPP__
#define __ORCUS_XLSX_TYPES_HPP__

#include "ooxml_types.hpp"

namespace orcus {

struct xlsx_rel_sheet_info : public opc_rel_extra
{
    pstring name;
    size_t  id;

    xlsx_rel_sheet_info() : id(0) {}

    virtual ~xlsx_rel_sheet_info() {}
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
