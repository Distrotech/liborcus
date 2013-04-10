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

#include "orcus/global.hpp"

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
        cout << "converted: year=" << ret.year << "; month=" << ret.month << "; day="
            << ret.day << "; hour=" << ret.hour << "; minute=" << ret.minute << "; second=" << ret.second << endl;
        assert(ret.year == tests[i].year);
        assert(ret.month == tests[i].month);
        assert(ret.day == tests[i].day);
        assert(ret.hour == tests[i].hour);
        assert(ret.minute == tests[i].minute);
        assert(ret.second == tests[i].second);
    }
}

int main()
{
    test_date_time_conversion();
    return EXIT_SUCCESS;
}
