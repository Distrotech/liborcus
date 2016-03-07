/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/parser_global.hpp"

namespace orcus { namespace json {

namespace {

const char quote = '"';
const char backslash = '\\';

}

void dump_string(std::ostringstream& os, const std::string& s)
{
    os << quote;
    for (auto it = s.begin(), ite = s.end(); it != ite; ++it)
    {
        char c = *it;
        if (c == '"')
            // Escape double quote, but not forward slash.
            os << backslash;
        else if (c == backslash)
        {
            // Escape a '\' if and only if the next character is not one of control characters.
            auto itnext = it + 1;
            if (itnext == ite || get_string_escape_char_type(*itnext) != string_escape_char_t::control_char)
                os << backslash;
        }
        os << c;
    }
    os << quote;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
