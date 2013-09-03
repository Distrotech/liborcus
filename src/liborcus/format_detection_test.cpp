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

#include "orcus/format_detection.hpp"
#include "orcus/stream.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace orcus;
using namespace std;

void test_detect_formats()
{
    struct {
        const char* path; format_t format;
    } tests[] = {
        { SRCDIR"/test/ods/raw-values-1/input.ods", format_ods },
        { SRCDIR"/test/xlsx/raw-values-1/input.xlsx", format_xlsx },
        { SRCDIR"/test/xls-xml/basic/input.xml", format_xls_xml },
        { SRCDIR"/test/gnumeric/test.gnumeric", format_gnumeric }
    };

    size_t n = sizeof(tests[0]) / sizeof(tests);
    for (size_t i = 0; i < n; ++i)
    {
        string strm;
        load_file_content(tests[i].path, strm);
        assert(!strm.empty());
        format_t detected = detect(reinterpret_cast<const unsigned char*>(&strm[0]), strm.size());
        assert(detected == tests[i].format);
    }
}

int main()
{
    test_detect_formats();
    return EXIT_SUCCESS;
}
