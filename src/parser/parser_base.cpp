/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/parser_base.hpp"
#include "orcus/parser_global.hpp"

namespace orcus {

parser_base::parser_base(const char* p, size_t n) :
    mp_char(p), m_pos(0), m_length(n) {}

void parser_base::next()
{
    ++m_pos;
    ++mp_char;
}

char parser_base::cur_char() const
{
    return *mp_char;
}

char parser_base::next_char() const
{
    return *(mp_char+1);
}

void parser_base::skip(const char* chars_to_skip)
{
    for (; has_char(); next())
    {
        if (!is_in(*mp_char, chars_to_skip))
            break;
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
