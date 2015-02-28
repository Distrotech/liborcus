/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CSS_TYPES_HPP
#define INCLUDED_ORCUS_CSS_TYPES_HPP

#include "orcus/env.hpp"
#include "orcus/pstring.hpp"

#include <vector>
#include <boost/unordered_set.hpp>

namespace orcus {

enum css_combinator_t
{
    css_combinator_descendant,   /// 'E F' where F is a descendant of E.
    css_combinator_child,        /// 'E > F' where F is a direct child of E.
    css_combinator_next_sibling  /// 'E + F' where F is a direct sibling of E where E precedes F.
};

struct css_simple_selector_t
{
    pstring name;
    pstring identifier;
    boost::unordered_set<pstring> classes;
};

struct css_chained_simple_selector_t
{
    css_combinator_t combinator;
    css_simple_selector_t selector;
};

/**
 * Each CSS selector consists of one or more chained simple selectors.
 */
struct css_selector_t
{
    css_simple_selector_t first;
    std::vector<css_chained_simple_selector_t> chained;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
