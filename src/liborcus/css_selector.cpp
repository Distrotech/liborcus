/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_selector.hpp"

namespace orcus {

css_simple_selector_t::css_simple_selector_t() {}

void css_simple_selector_t::clear()
{
    name.clear();
    id.clear();
    classes.clear();
}

bool css_simple_selector_t::empty() const
{
    return name.empty() && id.empty() && classes.empty();
}

bool css_simple_selector_t::operator== (const css_simple_selector_t& r) const
{
    if (name != r.name)
        return false;

    if (id != r.id)
        return false;

    if (classes != r.classes)
        return false;

    return true;
}

bool css_simple_selector_t::operator!= (const css_simple_selector_t& r) const
{
    return !operator==(r);
}

size_t css_simple_selector_t::hash::operator() (const css_simple_selector_t& ss) const
{
    static pstring::hash hasher;

    size_t val = hasher(ss.name);
    val += hasher(ss.id);
    classes_type::const_iterator it = ss.classes.begin(), ite = ss.classes.end();
    for (; it != ite; ++it)
        val += hasher(*it);

    return val;
}

bool css_chained_simple_selector_t::operator== (const css_chained_simple_selector_t& r) const
{
    return combinator == r.combinator && simple_selector == r.simple_selector;
}

void css_selector_t::clear()
{
    first.clear();
    chained.clear();
}

bool css_selector_t::operator== (const css_selector_t& r) const
{
    return first == r.first && chained == r.chained;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
