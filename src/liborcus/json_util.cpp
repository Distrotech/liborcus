/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_global.hpp"

#include <sstream>

namespace orcus { namespace json {

namespace {

const char quote = '"';

}

void dump_string(std::ostringstream& os, const std::string& s)
{
    os << quote << escape_string(s) << quote;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
