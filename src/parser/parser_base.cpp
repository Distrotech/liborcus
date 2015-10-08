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
#include <limits>
#include <cassert>

namespace orcus {

parse_error::parse_error(const std::string& msg, std::ptrdiff_t offset) :
    general_error(msg), m_offset(offset) {}

std::ptrdiff_t parse_error::offset() const
{
    return m_offset;
}

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
    mp_begin(p), mp_char(p), mp_end(p+n)
{
}

void parser_base::next(size_t inc)
{
    mp_char += inc;
}

void parser_base::prev(size_t dec)
{
    mp_char -= dec;
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

double parser_base::parse_double()
{
    size_t max_length = remaining_size();
    const char* p = mp_char;
    double val = parse_numeric(p, max_length);
    if (p == mp_char)
        return std::numeric_limits<double>::quiet_NaN();

    mp_char = p;
    return val;
}

size_t parser_base::remaining_size() const
{
    return std::distance(mp_char, mp_end) - 1;
}

std::ptrdiff_t parser_base::offset() const
{
    return std::distance(mp_begin, mp_char);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
