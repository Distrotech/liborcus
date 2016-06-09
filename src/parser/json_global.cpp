/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_global.hpp"
#include "orcus/parser_global.hpp"

#include <sstream>

namespace orcus { namespace json {

namespace {

const char backslash = '\\';

}

std::string escape_string(const std::string& input)
{
    std::ostringstream os;

    for (auto it = input.begin(), ite = input.end(); it != ite; ++it)
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

    return os.str();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
