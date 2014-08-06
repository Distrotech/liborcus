/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_FORMULA_GLOBAL_HPP
#define ORCUS_SPREADSHEET_FORMULA_GLOBAL_HPP

#include <cstdlib>

namespace ixion {

struct abs_range_t;
class formula_name_resolver;

}

namespace orcus { namespace spreadsheet {

/**
 * Parse a string representing a 2-dimensional range using the passed name
 * resolver, and return an absolute range object.  The sheet index will be
 * unconditionally set to 0.  It returns an invalid range object in case the
 * parsing fails.
 *
 * @param resolver name resolver to use to resolve the range string.
 * @param p_ref pointer to the first character of the range string.
 * @param n_ref length of the range string.
 *
 * @return absolute range object, which may be set invalid in case the
 *         parsing is unsuccessful.
 */
ixion::abs_range_t to_abs_range(
    const ixion::formula_name_resolver& resolver, const char* p_ref, size_t n_ref);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
