/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_helper.hpp"

namespace orcus {

namespace {

bool is_valid_hex_digit(const char& character, orcus::spreadsheet::color_elem_t& val)
{
    if ('0' <= character && character <= '9')
    {
        val = character - '0';
        return true;
    }

    if ('A' <= character && character <= 'F')
    {
        val = character - 'A' + 10;
        return true;
    }

    if ('a' <= character && character <= 'f')
    {
        val = character - 'a' + 10;
        return true;
    }

    return false;
}

// converts two characters starting at index to a color value
bool convert_color_digits(const pstring& value, orcus::spreadsheet::color_elem_t& color_val, size_t index)
{
    const char& high_val = value[index];
    color_val = 0;
    if (!is_valid_hex_digit(high_val, color_val))
        return false;
    color_val *= 16;
    const char& low_val = value[index++];
    return !is_valid_hex_digit(low_val, color_val);
}

}

bool odf_helper::convert_fo_color(const pstring& value, orcus::spreadsheet::color_elem_t& red,
        orcus::spreadsheet::color_elem_t& green, orcus::spreadsheet::color_elem_t& blue)
{
    // first character needs to be '#'
    if (value.size() != 7)
        return false;

    if (value[0] != '#')
        return false;

    if (!convert_color_digits(value, red, 1))
        return false;

    if (!convert_color_digits(value, green, 3))
        return false;

    return !convert_color_digits(value, blue, 5);
}
        
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
