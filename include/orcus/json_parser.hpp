/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_HPP
#define INCLUDED_ORCUS_JSON_PARSER_HPP

#include "orcus/json_parser_base.hpp"

#include <cassert>
#include <cmath>

namespace orcus {

template<typename _Handler>
class json_parser : public json::parser_base
{
public:
    typedef _Handler handler_type;

    json_parser(const char* p, size_t n, handler_type& hdl);

    void parse();

private:
    void root_value();
    void value();
    void array();
    void object();
    void number();
    void number_with_exp(double base);
    void string();

private:
    handler_type& m_handler;
};

template<typename _Handler>
json_parser<_Handler>::json_parser(
    const char* p, size_t n, handler_type& hdl) :
    json::parser_base(p, n), m_handler(hdl) {}

template<typename _Handler>
void json_parser<_Handler>::parse()
{
    m_handler.begin_parse();

    skip_blanks();
    if (has_char())
        root_value();

    if (has_char())
        throw json::parse_error("parse: unexpected trailing string segment.");

    m_handler.end_parse();
}

template<typename _Handler>
void json_parser<_Handler>::root_value()
{
    char c = cur_char();

    switch (c)
    {
        case '[':
            array();
        break;
        case '{':
            object();
        break;
        default:
            json::parse_error::throw_with(
                "root_value: either '[' or '{' was expected, but '", cur_char(), "' was found.");
    }
}

template<typename _Handler>
void json_parser<_Handler>::value()
{
    char c = cur_char();
    if (is_numeric(c))
    {
        number();
        return;
    }

    switch (c)
    {
        case '-':
            number();
        break;
        case '[':
            array();
        break;
        case '{':
            object();
        break;
        case 't':
            parse_true();
            m_handler.boolean_true();
        break;
        case 'f':
            parse_false();
            m_handler.boolean_false();
        break;
        case 'n':
            parse_null();
            m_handler.null();
        break;
        case '"':
            string();
        break;
        default:
            json::parse_error::throw_with("value: failed to parse '", cur_char(), "'.");
    }
}

template<typename _Handler>
void json_parser<_Handler>::array()
{
    assert(cur_char() == '[');

    m_handler.begin_array();
    for (next(); has_char(); next())
    {
        skip_blanks();
        value();
        skip_blanks();
        if (has_char())
        {
            switch (cur_char())
            {
                case ']':
                    m_handler.end_array();
                    next();
                    skip_blanks();
                    return;
                case ',':
                    continue;
                default:
                    json::parse_error::throw_with("array: either ']' or ',' expected, but '", cur_char(), "' found.");
            }
        }
    }

    throw json::parse_error("array: failed to parse array.");
}

template<typename _Handler>
void json_parser<_Handler>::object()
{
    assert(cur_char() == '{');

    m_handler.begin_object();
    for (next(); has_char(); next())
    {
        skip_blanks();
        if (!has_char())
            throw json::parse_error("object: stream ended prematurely before reaching a key.");

        if (cur_char() != '"')
            json::parse_error::throw_with("object: '\"' was expected, but '", cur_char(), "' found.");

        parse_string_state res = parse_string();
        if (!res.str)
        {
            // Parsing was unsuccessful.
            switch (res.length)
            {
                case parse_string_error_no_closing_quote:
                    throw json::parse_error("object: stream ended prematurely before reaching the closing quote of a key.");
                case parse_string_error_illegal_escape_char:
                    json::parse_error::throw_with("object: illegal escape character '", cur_char(), "' in key value.");
                default:
                    throw json::parse_error("object: unknown error while parsing a key value.");
            }
        }

        m_handler.object_key(res.str, res.length);

        skip_blanks();
        if (cur_char() != ':')
            json::parse_error::throw_with("object: ':' was expected, but '", cur_char(), "' found.");

        next();
        skip_blanks();

        if (!has_char())
            throw json::parse_error("object: stream ended prematurely before reaching a value.");

        value();

        skip_blanks();
        if (!has_char())
            throw json::parse_error("object: stream ended prematurely before reaching either ']' or ','.");

        switch (cur_char())
        {
            case '}':
                m_handler.end_object();
                next();
                skip_blanks();
                return;
            case ',':
                continue;
            default:
                json::parse_error::throw_with("object: either ']' or ',' expected, but '", cur_char(), "' found.");
        }
    }

    throw json::parse_error("object: closing '}' was never reached.");
}

template<typename _Handler>
void json_parser<_Handler>::number()
{
    assert(is_numeric(cur_char()) || cur_char() == '-');

    double val = parse_double_or_throw();
    switch (cur_char())
    {
        case 'e':
        case 'E':
            number_with_exp(val);
            return;
        default:
            ;
    }
    m_handler.number(val);
    skip_blanks();
}

template<typename _Handler>
void json_parser<_Handler>::number_with_exp(double base)
{
    assert(cur_char() == 'e' || cur_char() == 'E');
    next();
    if (!has_char())
        throw json::parse_error("number_with_exp: illegal exponent value.");

    long exp = parse_long_or_throw();
    double v = std::pow(base, exp);
    m_handler.number(v);
    skip_blanks();
}

template<typename _Handler>
void json_parser<_Handler>::string()
{
    parse_string_state res = parse_string();
    if (res.str)
    {
        m_handler.string(res.str, res.length);
        return;
    }

    // Parsing was unsuccessful.
    switch (res.length)
    {
        case parse_string_error_no_closing_quote:
            throw json::parse_error("string: stream ended prematurely before reaching the closing quote.");
        case parse_string_error_illegal_escape_char:
            json::parse_error::throw_with("string: illegal escape character '", cur_char(), "'.");
        default:
            throw json::parse_error("string: unknown error.");
    }
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
