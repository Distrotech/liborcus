/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_types.hpp"

#include "orcus/global.hpp"

namespace orcus {

opc_rel_extra::~opc_rel_extra() {}

opc_rel_extras_t::opc_rel_extras_t() {}
opc_rel_extras_t::~opc_rel_extras_t()
{
    std::for_each(data.begin(), data.end(), map_object_deleter<map_type>());
}

void opc_rel_extras_t::swap(opc_rel_extras_t& other)
{
    data.swap(other.data);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
