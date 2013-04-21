/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef ORCUS_MEASUREMENT_HPP
#define ORCUS_MEASUREMENT_HPP

#include "env.hpp"

namespace orcus {

class pstring;

enum length_unit_t
{
    length_unit_unknown = 0,
    length_unit_centimeter,
    length_unit_inch,
    length_unit_point,
    length_unit_twip

    // TODO: Add more.
};

struct ORCUS_DLLPUBLIC length_t
{
    length_unit_t unit;
    double value;

    length_t();
};

/**
 * Parse a string value containing a part representing a numerical value
 * optionally followed by a part representing a unit of measurement.
 *
 * Examples of such string value are: "1.234in", "0.34cm" and so on.
 *
 * @param str original string value.
 *
 * @return structure containing a numerical value and a unit of measurement
 *         that the original string value represents.
 */
ORCUS_DLLPUBLIC length_t to_length(const pstring& str);

}

#endif
