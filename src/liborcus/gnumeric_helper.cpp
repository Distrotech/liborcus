/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gnumeric_helper.hpp"
#include "string_helper.hpp"

#include <cstdlib>
#include <vector>
#include <cassert>

namespace orcus {

using ::orcus::spreadsheet::color_elem_t;

namespace {

size_t parse_color_string(pstring str)
{
    unsigned long col_value = strtol( str.get(), nullptr, 16 );
    col_value = col_value >> 8;
    // make sure that this actually worked
    assert( col_value <= 255 );

    return static_cast<color_elem_t>(col_value);
}

}

bool gnumeric_helper::parse_RGB_color_attribute(color_elem_t& red, color_elem_t& green, color_elem_t& blue, const pstring& attr)
{
    std::vector<pstring> color = string_helper::split_string(attr, ':');

    if (color.size() != 3)
        return false;

    red = parse_color_string(color[0]);
    green = parse_color_string(color[1]);
    blue = parse_color_string(color[2]);

    return true;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
