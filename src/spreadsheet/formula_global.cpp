/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "formula_global.hpp"

#include <ixion/address.hpp>
#include <ixion/formula_name_resolver.hpp>

namespace orcus { namespace spreadsheet {

ixion::abs_range_t to_abs_range(
    const ixion::formula_name_resolver& resolver, const char* p_ref, size_t n_ref)
{
    ixion::abs_range_t range(ixion::abs_range_t::invalid);
    ixion::abs_address_t pos(0,0,0);

    ixion::formula_name_t res = resolver.resolve(p_ref, n_ref, pos);
    switch (res.type)
    {
        case ixion::formula_name_t::cell_reference:
            // Single cell reference.
            range.first = ixion::to_address(res.address).to_abs(pos);
            range.last = range.first;
        break;
        case ixion::formula_name_t::range_reference:
            // Range reference.
            range = ixion::to_range(res.range).to_abs(pos);
        break;
        default:
            ; // Unsupported range.  Leave it invalid.
    }

    return range;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
