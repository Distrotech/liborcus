/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_types.hpp"

namespace orcus {

opc_rel_extra::~opc_rel_extra() {}

opc_rel_extras_t::opc_rel_extras_t() {}
opc_rel_extras_t::~opc_rel_extras_t()
{
    map_type::iterator it = data.begin(), it_end = data.end();
    for (; it != it_end; ++it)
        delete it->second;
}

void opc_rel_extras_t::swap(opc_rel_extras_t& other)
{
    data.swap(other.data);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
