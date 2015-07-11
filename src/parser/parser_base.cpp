/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/parser_base.hpp"
#include "orcus/parser_global.hpp"

#include <sstream>
#include <cstring>

namespace orcus {

parse_error::parse_error(const std::string& msg) : general_error(msg) {}

std::string parse_error::build_message(const char* msg_before, char c, const char* msg_after)
{
    std::ostringstream os;

    if (msg_before)
        os << msg_before;

    os << c;

    if (msg_after)
        os << msg_after;

    return os.str();
}

std::string parse_error::build_message(
    const char* msg_before, const char* p, size_t n, const char* msg_after)
{
    std::ostringstream os;

    if (msg_before)
        os << msg_before;

    write_to(os, p, n);

    if (msg_after)
        os << msg_after;

    return os.str();
}

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

bool parser_base::parse_expected(const char* expected)
{
    size_t len = std::strlen(expected);
    if (len > remaining_size())
        return false;

    for (size_t i = 0; i < len; ++i, ++expected, next())
    {
        if (cur_char() != *expected)
            return false;
    }

    return true;
}

size_t parser_base::remaining_size() const
{
    return m_length - m_pos - 1;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
