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

#include "gnumeric_helper.hpp"

#include <cstdlib>
#include <vector>
#include <cassert>

namespace orcus {

using ::orcus::spreadsheet::color_elem_t;

namespace {

std::vector<pstring> split_string(pstring str, char sep)
{
    std::vector<pstring> ret;

    size_t len = 0;
    const char* start = str.get();
    for (size_t i = 0, n = str.size(); i < n; ++i)
    {
        if (str[i] == sep)
        {
            ret.push_back( pstring( start, len ) );
            len = 0;

            // if not at the end move the start string
            if (i < n-1)
                start = start + len + 1;

        }
        else
            ++len;
    }

    return ret;
}

size_t parse_color_string(pstring str)
{
    unsigned long col_value = strtol( str.get(), NULL, 16 );
    col_value = col_value >> 8;
    // make sure that this actually worked
    assert( col_value <= 255 );

    return static_cast<color_elem_t>(col_value);
}

}

bool gnumeric_helper::parse_RGB_color_attribute(color_elem_t& red, color_elem_t& green, color_elem_t& blue, const pstring& attr)
{
    std::vector<pstring> color = split_string(attr, ':');

    if (color.size() != 3)
        return false;

    red = parse_color_string(color[0]);
    green = parse_color_string(color[1]);
    blue = parse_color_string(color[2]);

    return true;
}

}
