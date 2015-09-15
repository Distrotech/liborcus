/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/parser_global.hpp"
#include "orcus/cell_buffer.hpp"

#include <cassert>

namespace orcus {

bool is_blank(char c)
{
    return is_in(c, " \t\n\r");
}

bool is_alpha(char c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

bool is_name_char(char c)
{
    return is_in(c, "-_");
}

bool is_numeric(char c)
{
    return ('0' <= c && c <= '9');
}

bool is_in(char c, const char* allowed)
{
    for (; *allowed != '\0'; ++allowed)
    {
        if (c == *allowed)
            return true;
    }
    return false;
}

void write_to(std::ostringstream& os, const char* p, size_t n)
{
    if (!p)
        return;

    const char* pend = p + n;
    for (; p != pend; ++p)
        os << *p;
}

double parse_numeric(const char*& p, size_t max_length)
{
    const char* p_end = p + max_length;
    double ret = 0.0, divisor = 1.0;
    bool negative_sign = false;
    bool before_decimal_pt = true;

    // Check for presence of a sign.
    if (p != p_end)
    {
        switch (*p)
        {
            case '+':
                ++p;
            break;
            case '-':
                negative_sign = true;
                ++p;
            break;
            default:
                ;
        }
    }

    for (; p != p_end; ++p)
    {
        if (*p == '.')
        {
            if (!before_decimal_pt)
            {
                // Second '.' encountered. Terminate the parsing.
                ret /= divisor;
                return negative_sign ? -ret : ret;
            }

            before_decimal_pt = false;
            continue;
        }

        if (*p < '0' || '9' < *p)
        {
            ret /= divisor;
            return negative_sign ? -ret : ret;
        }

        ret *= 10.0;
        ret += *p - '0';

        if (!before_decimal_pt)
            divisor *= 10.0;
    }

    ret /= divisor;
    return negative_sign ? -ret : ret;
}

long parse_integer(const char*& p, size_t max_length)
{
    const char* p_end = p + max_length;

    long ret = 0.0;
    bool negative_sign = false;

    // Check for presence of a sign.
    if (p != p_end)
    {
        switch (*p)
        {
            case '+':
                ++p;
            break;
            case '-':
                negative_sign = true;
                ++p;
            break;
            default:
                ;
        }
    }

    for (; p != p_end; ++p)
    {
        if (*p < '0' || '9' < *p)
            return negative_sign ? -ret : ret;

        ret *= 10;
        ret += *p - '0';
    }

    return negative_sign ? -ret : ret;
}

string_escape_char_t get_string_escape_char_type(char c)
{
    switch (c)
    {
        case '"':
        case '\\':
        case '/':
            return string_escape_char_t::valid;
        case 'b': // backspace
        case 'f': // formfeed
        case 'n': // newline
        case 'r': // carriage return
        case 't': // horizontal tab
            return string_escape_char_t::control_char;
        default:
            ;
    }

    return string_escape_char_t::invalid;
}

namespace {

parse_quoted_string_state parse_string_with_escaped_char(
    const char*& p, size_t max_length, const char* p_parsed, size_t n_parsed, char c,
    cell_buffer& buffer)
{
    const char* p_end = p + max_length;

    parse_quoted_string_state ret;
    ret.str = nullptr;
    ret.length = 0;

    // Start the buffer with the string we've parsed so far.
    buffer.reset();
    if (p_parsed && n_parsed)
        buffer.append(p_parsed, n_parsed);
    buffer.append(&c, 1);

    ++p;
    if (p == p_end)
    {
        ret.length = parse_quoted_string_state::error_no_closing_quote;
        return ret;
    }

    size_t len = 0;
    const char* p_head = p;
    bool escape = false;

    for (; p != p_end; ++p, ++len)
    {
        c = *p;

        if (escape)
        {
            escape = false;

            switch (get_string_escape_char_type(c))
            {
                case string_escape_char_t::valid:
                    buffer.append(p_head, len-1);
                    buffer.append(&c, 1);
                    ++p;
                    len = 0;
                    p_head = p;
                break;
                case string_escape_char_t::control_char:
                    // do nothing on control characters.
                break;
                case string_escape_char_t::invalid:
                default:
                    ret.length = parse_quoted_string_state::error_illegal_escape_char;
                    return ret;
            }
        }

        switch (*p)
        {
            case '"':
                // closing quote.
                buffer.append(p_head, len);
                ++p; // skip the quote.
                ret.str = buffer.get();
                ret.length = buffer.size();
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

    ret.length = parse_quoted_string_state::error_no_closing_quote;
    return ret;
}

parse_quoted_string_state parse_single_quoted_string_buffered(
    const char*& p, const char* p_end, cell_buffer& buffer)
{
    const char* p0 = p;
    size_t len = 0;
    char last = 0;
    char c = 0;

    for (; p != p_end; ++p)
    {
        if (!p0)
            p0 = p;

        c = *p;
        switch (c)
        {
            case '\'':
            {
                if (last == c)
                {
                    // Second "'" in series.  This is an encoded single quote.
                    buffer.append(p0, len);
                    p0 = nullptr;
                    last = 0;
                    len = 0;
                    continue;
                }
            }
            break;
            default:
            {
                if (last == '\'')
                {
                    buffer.append(p0, len-1);
                    parse_quoted_string_state ret;
                    ret.str = buffer.get();
                    ret.length = buffer.size();
                    return ret;
                }
            }
        }

        last = c;
        ++len;
    }

    if (last == '\'')
    {
        buffer.append(p0, len-1);
        parse_quoted_string_state ret;
        ret.str = buffer.get();
        ret.length = buffer.size();
        return ret;
    }

    parse_quoted_string_state ret;
    ret.str = nullptr;
    ret.length = parse_quoted_string_state::error_no_closing_quote;
    return ret;
}

}

parse_quoted_string_state parse_single_quoted_string(
    const char*& p, size_t max_length, cell_buffer& buffer)
{
    assert(*p == '\'');
    const char* p_end = p + max_length;
    ++p;

    parse_quoted_string_state ret;
    ret.str = p;
    ret.length = 0;

    if (p == p_end)
    {
        ret.str = nullptr;
        ret.length = parse_quoted_string_state::error_no_closing_quote;
        return ret;
    }

    char last = 0;
    char c = 0;
    for (; p != p_end; last = c, ++p, ++ret.length)
    {
        c = *p;
        switch (c)
        {
            case '\'':
            {
                if (last == c)
                {
                    // Encoded single quote.
                    buffer.reset();
                    buffer.append(ret.str, ret.length);
                    ++p;
                    return parse_single_quoted_string_buffered(p, p_end, buffer);
                }
            }
            break;
            default:
            {
                if (last == '\'')
                {
                    --ret.length;
                    return ret;
                }
            }

        }
    }

    if (last == '\'')
    {
        --ret.length;
        return ret;
    }

    ret.str = nullptr;
    ret.length = parse_quoted_string_state::error_no_closing_quote;
    return ret;
}

const char* parse_to_closing_single_quote(const char* p, size_t max_length)
{
    assert(*p == '\'');
    const char* p_end = p + max_length;
    ++p;

    if (p == p_end)
        return nullptr;

    char last = 0;
    char c = 0;
    for (; p != p_end; ++p)
    {
        c = *p;
        switch (c)
        {
            case '\'':
                if (last == '\'')
                {
                    last = 0;
                    continue;
                }
            break;
            default:
            {
                if (last == '\'')
                    return p;
            }
        }

        last = c;
    }

    if (last == '\'')
        return p;

    return nullptr;
}

parse_quoted_string_state parse_double_quoted_string(
    const char*& p, size_t max_length, cell_buffer& buffer)
{
    assert(*p == '"');
    const char* p_end = p + max_length;
    ++p;

    parse_quoted_string_state ret;
    ret.str = p;
    ret.length = 0;

    if (p == p_end)
    {
        ret.str = nullptr;
        ret.length = parse_quoted_string_state::error_no_closing_quote;
        return ret;
    }

    bool escape = false;

    for (; p != p_end; ++p, ++ret.length)
    {
        if (escape)
        {
            char c = *p;
            escape = false;

            switch (get_string_escape_char_type(c))
            {
                case string_escape_char_t::valid:
                    return parse_string_with_escaped_char(p, max_length, ret.str, ret.length-1, c, buffer);
                case string_escape_char_t::control_char:
                    // do nothing on control characters.
                break;
                case string_escape_char_t::invalid:
                default:
                    ret.str = nullptr;
                    ret.length = parse_quoted_string_state::error_illegal_escape_char;
                    return ret;
            }
        }

        switch (*p)
        {
            case '"':
                // closing quote.
                ++p; // skip the quote.
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

    ret.str = nullptr;
    ret.length = parse_quoted_string_state::error_no_closing_quote;
    return ret;
}

const char* parse_to_closing_double_quote(const char* p, size_t max_length)
{
    assert(*p == '"');
    const char* p_end = p + max_length;
    ++p;

    if (p == p_end)
        return nullptr;

    bool escape = false;

    for (; p != p_end; ++p)
    {
        if (escape)
        {
            char c = *p;
            escape = false;

            if (get_string_escape_char_type(c) == string_escape_char_t::invalid)
                return nullptr;
        }

        switch (*p)
        {
            case '"':
                // closing quote.
                ++p; // skip the quote.
                return p;
            case '\\':
                escape = true;
            break;
            default:
                ;
        }
    }

    return nullptr;
}

double clip(double input, double low, double high)
{
    if (input < low)
        input = low;
    if (input > high)
        input = high;
    return input;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
