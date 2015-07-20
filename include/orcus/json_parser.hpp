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
    void value();
    void array();
    void object();
    void number();
    void number_with_exp(double base);
    void string();
    void string_with_escaped_char(const char* p, size_t n, char c);

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
        value();

    m_handler.end_parse();
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

        string();
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
    assert(cur_char() == '"');
    next();
    if (!has_char())
        throw json::parse_error("string: stream ended prematurely before reaching the closing quote.");

    size_t len = 0;
    const char* p = mp_char;
    bool escape = false;

    for (; has_char(); next(), ++len)
    {
        if (escape)
        {
            char c = cur_char();
            escape = false;

            switch (json::get_escape_char_type(c))
            {
                case json::escape_char_t::legal:
                    string_with_escaped_char(p, len-1, c);
                    return;
                break;
                case json::escape_char_t::control_char:
                    // do nothing on control characters.
                break;
                case json::escape_char_t::illegal:
                default:
                    json::parse_error::throw_with("string: illegal escape character '", c, "'.");
            }
        }

        switch (cur_char())
        {
            case '"':
                // closing quote.
                m_handler.string(p, len);
                next(); // skip the quote.
                skip_blanks();
                return;
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

    throw json::parse_error("string: stream ended prematurely before reaching the closing quote.");
}

template<typename _Handler>
void json_parser<_Handler>::string_with_escaped_char(const char* p, size_t n, char c)
{
    // Start the buffer with the string we've parsed so far.
    reset_buffer();
    append_to_buffer(p, n);
    append_to_buffer(&c, 1);

    next();
    if (!has_char())
        throw json::parse_error(
            "string_with_escaped_char: stream ended prematurely before reaching the closing quote.");

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
                    json::parse_error::throw_with("string_with_escaped_char: illegal escape character '", c, "'.");
            }
        }

        switch (cur_char())
        {
            case '"':
                // closing quote.
                append_to_buffer(p, len);
                m_handler.string(get_buffer(), get_buffer_size());
                next(); // skip the quote.
                skip_blanks();
                return;
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

    throw json::parse_error(
        "string_with_escaped_char: stream ended prematurely before reaching the closing quote.");
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
