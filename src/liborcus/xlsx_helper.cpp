/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_helper.hpp"

namespace orcus {

bool to_rgb(
    const pstring& ps, spreadsheet::color_elem_t& alpha,
    spreadsheet::color_elem_t& red, spreadsheet::color_elem_t& green, spreadsheet::color_elem_t& blue)
{
    // RGB string is a 8-character string representing 32-bit hexadecimal
    // number e.g. 'FF004A12' (alpha - red - green - blue)
    size_t n = ps.size();
    if (n != 8)
        return false;

    unsigned long v = strtoul(ps.get(), nullptr, 16);
    blue  = (0x000000FF & v);
    green = (0x000000FF & (v >> 8));
    red   = (0x000000FF & (v >> 16));
    alpha = (0x000000FF & (v >> 24));

    return true;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
