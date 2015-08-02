/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
        { SRCDIR"/test/ods/raw-values-1/input.ods",   format_t::ods },
        { SRCDIR"/test/xlsx/raw-values-1/input.xlsx", format_t::xlsx },
        { SRCDIR"/test/xls-xml/basic/input.xml",      format_t::xls_xml },
        { SRCDIR"/test/gnumeric/test.gnumeric",       format_t::gnumeric }
    };

    size_t n = sizeof(tests[0]) / sizeof(tests);
    for (size_t i = 0; i < n; ++i)
    {
        string strm = load_file_content(tests[i].path);
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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
