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

bool is_in(char c, const char* allowed)
{
    for (; *allowed != '\0'; ++allowed)
    {
        if (c == *allowed)
            return true;
    }
    return false;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
