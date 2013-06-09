/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "orcus/sax_parser_global.hpp"

#include <cstring>

namespace orcus { namespace sax {

char decode_xml_encoded_char(const char* p, size_t n)
{
    if (n == 2)
    {
        if (!std::strncmp(p, "lt", n))
            return '<';
        else if (!std::strncmp(p, "gt", n))
            return '>';
        else
            return '\0';
    }
    else if (n == 3)
    {
        if (!std::strncmp(p, "amp", n))
            return '&';
        else
            return '\0';
    }
    else if (n == 4)
    {
        if (!std::strncmp(p, "apos", n))
            return '\'';
        else if (!std::strncmp(p, "quot", 4))
            return '"';
        else
            return '\0';
    }

    return '\0';
}

bool is_blank(char c)
{
    if (c == ' ')
        return true;

    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;

    if (c == '\t')
        // tab character.
        return true;

    return false;
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
    switch (c)
    {
        case '-':
        case '_':
            return true;
    }

    return false;
}

bool is_numeric(char c)
{
    return ('0' <= c && c <= '9');
}

}}
