/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/stream.hpp"

#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>

using namespace std;
using namespace orcus;

void test_stream_create_error_output()
{
    string output = create_parse_error_output("{}", 1);
    cout << output << endl;
    const char* expected = "1:2: {}\n      ^";
    assert(output == expected);
}

int main()
{
    test_stream_create_error_output();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
