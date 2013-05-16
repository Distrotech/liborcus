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

#include "orcus/measurement.hpp"
#include "orcus/pstring.hpp"
#include "orcus/exception.hpp"

#include <sstream>

using namespace std;

namespace orcus {

namespace {

double parse_numeric(const char*& p, const char* p_end)
{
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

long parse_integer(const char*& p, const char* p_end)
{
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

}

length_t::length_t() : unit(length_unit_unknown), value(0.0) {}

std::string length_t::print() const
{
    ostringstream os;
    os << value;

    switch (unit)
    {
        case length_unit_centimeter:
            os << " cm";
        break;
        case length_unit_inch:
            os << " in";
        break;
        case length_unit_point:
            os << " pt";
        break;
        case length_unit_twip:
            os << " twip";
        break;
        case length_unit_unknown:
        default:
            ;
    }

    return os.str();
}

double to_double(const char* p, const char* p_end, const char** p_parse_ended)
{
    double val = parse_numeric(p, p_end);
    if (p_parse_ended)
        *p_parse_ended = p;

    return val;
}

long to_long(const char* p, const char* p_end, const char** p_parse_ended)
{
    long val = parse_integer(p, p_end);
    if (p_parse_ended)
        *p_parse_ended = p;

    return val;
}

length_t to_length(const pstring& str)
{
    length_t ret;
    if (str.empty())
        return ret;

    const char* p = str.get();
    const char* p_start = p;
    const char* p_end = p_start + str.size();
    ret.value = parse_numeric(p, p_end);

    // TODO: See if this part can be optimized.
    pstring tail(p, p_end-p);
    if (tail == "in")
        ret.unit = length_unit_inch;
    else if (tail == "cm")
        ret.unit = length_unit_centimeter;

    return ret;
}

namespace {

double convert_inch(double value, length_unit_t unit_to)
{
    switch (unit_to)
    {
        case length_unit_twip:
            // inches to twips : 1 twip = 1/1440 inches
            return value * 1440.0;
        default:
            ;
    }

    throw general_error("convert_inch: unsupported unit of measurement.");
}

double convert_centimeter(double value, length_unit_t unit_to)
{
    switch (unit_to)
    {
        case length_unit_twip:
            // centimeters to twips : 2.54 cm = 1 inch = 1440 twips
            return value / 2.54 * 1440.0;
        default:
            ;
    }

    throw general_error("convert_centimeter: unsupported unit of measurement.");
}

/**
 * Since Excel's column width is based on the maximum digit width of font
 * used as the "Normal" style font, it's impossible to convert it accurately
 * without the font information.
 */
double convert_xlsx_column_digit(double value, length_unit_t unit_to)
{
    // Convert to centimeters first. Here, we'll just assume that a single
    // digit always equals 2 millimeters. TODO: find a better way to convert
    // this.
    value *= 0.2;
    return convert_centimeter(value, unit_to);
}

}

double convert(double value, length_unit_t unit_from, length_unit_t unit_to)
{
    switch (unit_from)
    {
        case length_unit_inch:
            return convert_inch(value, unit_to);
        case length_unit_centimeter:
            return convert_centimeter(value, unit_to);
        case length_unit_xlsx_column_digit:
            return convert_xlsx_column_digit(value, unit_to);
        default:
            ;
    }
    throw general_error("convert: unsupported unit of measurement.");
}

}
