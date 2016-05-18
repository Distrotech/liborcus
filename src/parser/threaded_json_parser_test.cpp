/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/threaded_json_parser.hpp>

using namespace orcus;

void test_threaded_json_parser_1()
{
    class handler
    {
    public:
        void begin_parse() {}
        void end_parse() {}
        void begin_array() {}
        void end_array() {}
        void begin_object() {}
        void object_key(const char* p, size_t len, bool transient) {}
        void end_object() {}
        void boolean_true() {}
        void boolean_false() {}
        void null() {}
        void string(const char* p, size_t len, bool transient) {}
        void number(double val) {}
    };

    handler hdl;
    threaded_json_parser<handler> parser(nullptr, 0, hdl);
    parser.parse();
}

int main()
{
    test_threaded_json_parser_1();
    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
