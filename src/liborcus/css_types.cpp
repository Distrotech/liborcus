/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_types.hpp"

namespace orcus {

void css_simple_selector_t::clear()
{
    name.clear();
    identifier.clear();
    classes.clear();
}

bool css_simple_selector_t::empty() const
{
    return name.empty() && identifier.empty() && classes.empty();
}

void css_selector_t::clear()
{
    first.clear();
    chained.clear();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
