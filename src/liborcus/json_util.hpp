/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_UTIL_HPP
#define INCLUDED_ORCUS_JSON_UTIL_HPP

#include <sstream>

namespace orcus { namespace json {

void dump_string(std::ostringstream& os, const std::string& s);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
