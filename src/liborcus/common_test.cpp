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

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cmath>

#include "orcus/global.hpp"
#include "orcus/measurement.hpp"

using namespace std;
using namespace orcus;

void test_date_time_conversion()
{
    struct {
        const char* str;
        int year;
        int month;
        int day;
        int hour;
        int minute;
        double second;
    } tests[] = {
        { "2011-02-12", 2011, 2, 12, 0, 0, 0.0 },
        { "1934-12-23T21:34:56.69", 1934, 12, 23, 21, 34, 56.69 },
    };

    for (size_t i = 0, n = sizeof(tests)/sizeof(tests[0]); i < n; ++i)
    {
        pstring str(tests[i].str);
        date_time_t ret = to_date_time(str);
        cout << "original: " << str << endl;
        cout << "converted: year=" << ret.year << ", month=" << ret.month << ", day="
            << ret.day << ", hour=" << ret.hour << ", minute=" << ret.minute << ", second=" << ret.second << endl;
        assert(ret.year == tests[i].year);
        assert(ret.month == tests[i].month);
        assert(ret.day == tests[i].day);
        assert(ret.hour == tests[i].hour);
        assert(ret.minute == tests[i].minute);
        assert(ret.second == tests[i].second);
    }
}

string to_string(length_unit_t unit)
{
    switch (unit)
    {
        case length_unit_centimeter:
            return "centimeter";
        case length_unit_inch:
            return "inch";
        case length_unit_point:
            return "point";
        case length_unit_twip:
            return "twip";
        case length_unit_unknown:
        default:
            ;
    }
    return "unknown";
}

void test_measurement_conversion()
{
    struct {
        const char* str;
        double expected;
        size_t decimals;
        length_unit_t unit;
    } tests[] = {
        { "12.34", 12.34, 2, length_unit_unknown },
        { "35", 35, 0, length_unit_unknown },
        { "0.69825", 0.69825, 5, length_unit_unknown },
        { ".1592", 0.1592, 4, length_unit_unknown },
        { "5", 5.0, 0, length_unit_unknown },
        { "-3", -3.0, 0, length_unit_unknown },
        { "-3.456", -3.456, 3, length_unit_unknown },
        { "-.987", -0.987, 3, length_unit_unknown },
        { "-100.987.", -100.987, 3, length_unit_unknown }, // Second decimal point should stop the parsing.

        { "12.345in", 12.345, 3, length_unit_inch },
        { "120.30001cm", 120.30001, 5, length_unit_centimeter },
    };

    for (size_t i = 0, n = sizeof(tests)/sizeof(tests[0]); i < n; ++i)
    {
        length_t ret = to_length(tests[i].str);
        cout << "original: '" << tests[i].str << "', converted: " << ret.value
            << " (" << to_string(ret.unit) << "), expected: "
            << tests[i].expected << " (" << to_string(tests[i].unit) << ")" << endl;

        // Check for double-precision equality without the rounding error.
        double factor = 1.0;
        for (size_t j = 0; j < tests[i].decimals; ++j)
            factor *= 10.0;

        double converted = round(ret.value * factor);
        double expected = round(tests[i].expected * factor);
        assert(converted == expected);

        assert(ret.unit == tests[i].unit);
    }
}

void test_string2number_conversion()
{
    struct {
        const char* str;
        double expected;
        size_t decimals;
        size_t end_pos;
    } tests[] = {
        { "1.2", 1.2, 1, 3 },
        { "1.2a", 1.2, 1, 3 },
        { "1.2.", 1.2, 1, 3 },
        { "1.3456", 1.3456, 4, 6 },
        { "-10.345", -10.345, 3, 7 },
        { "-10.345-", -10.345, 3, 7 },
    };

    for (size_t i = 0, n = sizeof(tests)/sizeof(tests[0]); i < n; ++i)
    {
        const char* p = tests[i].str;
        const char* p_end = p + strlen(p);
        const char* p_parse_ended = NULL;
        double converted = to_double(p, p_end, &p_parse_ended);
        cout << "original: '" << tests[i].str << "', converted: " << converted
            << ", expected: " << tests[i].expected << endl;

        // Check for double-precision equality without the rounding error.
        double factor = 1.0;
        for (size_t j = 0; j < tests[i].decimals; ++j)
            factor *= 10.0;

        converted = round(converted * factor);
        double expected = round(tests[i].expected * factor);
        assert(converted == expected);

        // Check the end parse position.
        const char* pos_expected = p + tests[i].end_pos;
        assert(pos_expected == p_parse_ended);
    }
}

void test_string2long_conversion()
{
    struct {
        const char* str;
        long expected;
        size_t end_pos;
    } tests[] = {
        { "1", 1, 1 },
        { "12", 12, 2 },
        { "13.4", 13, 2 },
        { "-23", -23, 3 },
        { "678abc", 678, 3 },
    };

    for (size_t i = 0, n = sizeof(tests)/sizeof(tests[0]); i < n; ++i)
    {
        const char* p = tests[i].str;
        const char* p_end = p + strlen(p);
        const char* p_parse_ended = NULL;
        long converted = to_long(p, p_end, &p_parse_ended);
        cout << "original: '" << tests[i].str << "', converted: " << converted
            << ", expected: " << tests[i].expected << endl;

        assert(converted == tests[i].expected);

        // Check the end parse position.
        const char* pos_expected = p + tests[i].end_pos;
        assert(pos_expected == p_parse_ended);
    }
}

int main()
{
    test_date_time_conversion();
    test_measurement_conversion();
    test_string2number_conversion();
    test_string2long_conversion();
    return EXIT_SUCCESS;
}
