/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/parser_global.hpp"

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
