/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_parser_base.hpp"
#include "orcus/global.hpp"
#include "orcus/cell_buffer.hpp"

#include <cassert>
#include <cmath>

namespace orcus { namespace json {

parse_error::parse_error(const std::string& msg, std::ptrdiff_t offset) :
    ::orcus::parse_error(msg, offset) {}

void parse_error::throw_with(
    const char* msg_before, char c, const char* msg_after, std::ptrdiff_t offset)
{
    throw parse_error(build_message(msg_before, c, msg_after), offset);
}

void parse_error::throw_with(
    const char* msg_before, const char* p, size_t n, const char* msg_after, std::ptrdiff_t offset)
{
    throw parse_error(build_message(msg_before, p, n, msg_after), offset);
}

struct parser_base::impl
{
    cell_buffer m_buffer;
};

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(orcus::make_unique<impl>()) {}

parser_base::~parser_base() {}

void parser_base::parse_true()
{
    static const char* expected = "true";
    if (!parse_expected(expected))
        throw parse_error("parse_true: boolean 'true' expected.", offset());

    skip_blanks();
}

void parser_base::parse_false()
{
    static const char* expected = "false";
    if (!parse_expected(expected))
        throw parse_error("parse_false: boolean 'false' expected.", offset());

    skip_blanks();
}

void parser_base::parse_null()
{
    static const char* expected = "null";
    if (!parse_expected(expected))
        throw parse_error("parse_null: null expected.", offset());

    skip_blanks();
}

long parser_base::parse_long_or_throw()
{
    const char* p = mp_char;
    long v = parse_integer(p, remaining_size());
    if (p == mp_char)
        throw parse_error("parse_integer_or_throw: failed to parse long integer value.", offset());

    mp_char = p;
    return v;
}

double parser_base::parse_double_or_throw()
{
    double v = parse_double();
    if (std::isnan(v))
        throw parse_error("parse_double_or_throw: failed to parse double precision value.", offset());
    return v;
}

parse_quoted_string_state parser_base::parse_string()
{
    assert(cur_char() == '"');
    size_t max_length = remaining_size();
    const char* p = mp_char;
    parse_quoted_string_state ret = parse_double_quoted_string(p, max_length, mp_impl->m_buffer);
    mp_char = p;

    if (ret.str)
        skip_blanks();

    return ret;
}

void parser_base::skip_blanks()
{
    for (; mp_char != mp_end && *mp_char <= ' '; ++mp_char)
        ;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
