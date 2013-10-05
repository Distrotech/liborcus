/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_GNUMERIC_HELPER__
#define __ORCUS_GNUMERIC_HELPER__

#include "orcus/spreadsheet/types.hpp"
#include "orcus/pstring.hpp"

namespace orcus {

/**
 *  This class provides common helper functions for the gnumeric parser
 */
class gnumeric_helper
{
public:

    /**
     * Parses a RGB color attribute string and returns the single components
     *
     * @return returns false if the string could not be parsed as valid color
     * attribute
     */
    static bool parse_RGB_color_attribute(spreadsheet::color_elem_t& red,
            spreadsheet::color_elem_t& green,
            spreadsheet::color_elem_t& blue, const pstring& attr);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
