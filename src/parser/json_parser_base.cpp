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
#include <iostream>

namespace orcus { namespace json {

escape_char_t get_escape_char_type(char c)
{
    switch (c)
    {
        case '"':
        case '\\':
        case '/':
            return escape_char_t::legal;
        case 'b': // backspace
        case 'f': // formfeed
        case 'n': // newline
        case 'r': // carriage return
        case 't': // horizontal tab
            return escape_char_t::control_char;
        default:
            ;
    }

    return escape_char_t::illegal;
}

parse_error::parse_error(const std::string& msg) : ::orcus::parse_error(msg) {}

void parse_error::throw_with(const char* msg_before, char c, const char* msg_after)
{
    throw parse_error(build_message(msg_before, c, msg_after));
}

void parse_error::throw_with(
    const char* msg_before, const char* p, size_t n, const char* msg_after)
{
    throw parse_error(build_message(msg_before, p, n, msg_after));
}

struct parser_base::impl
{
    cell_buffer m_buffer;
};

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(make_unique<impl>()) {}

parser_base::~parser_base() {}

void parser_base::parse_true()
{
    static const char* expected = "true";
    if (!parse_expected(expected))
        throw parse_error("parse_true: boolean 'true' expected.");

    skip_blanks();
}

void parser_base::parse_false()
{
    static const char* expected = "false";
    if (!parse_expected(expected))
        throw parse_error("parse_false: boolean 'false' expected.");

    skip_blanks();
}

void parser_base::parse_null()
{
    static const char* expected = "null";
    if (!parse_expected(expected))
        throw parse_error("parse_null: null expected.");

    skip_blanks();
}

long parser_base::parse_long_or_throw()
{
    const char* p = mp_char;
    long v = parse_integer(p, remaining_size());
    if (p == mp_char)
        throw parse_error("parse_integer_or_throw: failed to parse long integer value.");

    m_pos += p - mp_char;
    mp_char = p;
    return v;
}

double parser_base::parse_double_or_throw()
{
    double v = parse_double();
    if (std::isnan(v))
        throw parse_error("parse_double_or_throw: failed to parse double precision value.");
    return v;
}

parser_base::parse_string_state parser_base::parse_string()
{
    assert(cur_char() == '"');
    next();

    parse_string_state ret;
    ret.str = mp_char;
    ret.length = 0;

    if (!has_char())
    {
        ret.str = NULL;
        ret.length = parse_string_error_no_closing_quote;
        return ret;
    }

    bool escape = false;

    for (; has_char(); next(), ++ret.length)
    {
        if (escape)
        {
            char c = cur_char();
            escape = false;

            switch (json::get_escape_char_type(c))
            {
                case json::escape_char_t::legal:
                    return parse_string_with_escaped_char(ret.str, ret.length-1, c);
                case json::escape_char_t::control_char:
                    // do nothing on control characters.
                break;
                case json::escape_char_t::illegal:
                default:
                    ret.str = NULL;
                    ret.length = parse_string_error_illegal_escape_char;
                    return ret;
            }
        }

        switch (cur_char())
        {
            case '"':
                // closing quote.
                next(); // skip the quote.
                skip_blanks();
                return ret;
            break;
            case '\\':
            {
                escape = true;
                continue;
            }
            break;
            default:
                ;
        }
    }

    ret.str = NULL;
    ret.length = parse_string_error_no_closing_quote;
    return ret;
}

parser_base::parse_string_state parser_base::parse_string_with_escaped_char(
    const char* p, size_t n, char c)
{
    parse_string_state ret;
    ret.str = NULL;
    ret.length = 0;

    // Start the buffer with the string we've parsed so far.
    reset_buffer();
    append_to_buffer(p, n);
    append_to_buffer(&c, 1);

    next();
    if (!has_char())
    {
        ret.length = parse_string_error_no_closing_quote;
        return ret;
    }

    size_t len = 0;
    p = mp_char;
    bool escape = false;

    for (; has_char(); next(), ++len)
    {
        char c = cur_char();

        if (escape)
        {
            escape = false;

            switch (json::get_escape_char_type(c))
            {
                case json::escape_char_t::legal:
                    append_to_buffer(p, len-1);
                    append_to_buffer(&c, 1);
                    next();
                    len = 0;
                    p = mp_char;
                break;
                case json::escape_char_t::control_char:
                    // do nothing on control characters.
                break;
                case json::escape_char_t::illegal:
                default:
                    ret.length = parse_string_error_illegal_escape_char;
                    return ret;
            }
        }

        switch (cur_char())
        {
            case '"':
                // closing quote.
                append_to_buffer(p, len);
                next(); // skip the quote.
                skip_blanks();
                ret.str = get_buffer();
                ret.length = get_buffer_size();
                return ret;
            break;
            case '\\':
            {
                escape = true;
                continue;
            }
            break;
            default:
                ;
        }
    }

    ret.length = parse_string_error_no_closing_quote;
    return ret;
}

void parser_base::skip_blanks()
{
    skip(" \t\n\r");
}

void parser_base::reset_buffer()
{
    mp_impl->m_buffer.reset();
}

void parser_base::append_to_buffer(const char* p, size_t n)
{
    if (!p || !n)
        return;

    mp_impl->m_buffer.append(p, n);
}

const char* parser_base::get_buffer() const
{
    return mp_impl->m_buffer.get();
}

size_t parser_base::get_buffer_size() const
{
    return mp_impl->m_buffer.size();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
