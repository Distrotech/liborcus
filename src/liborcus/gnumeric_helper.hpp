/*************************************************************************
 *
 * Copyright (c) 2012 Markus Mohrhard
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
