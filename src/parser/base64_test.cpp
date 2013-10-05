/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/base64.hpp"

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <string>
#include <iostream>

using namespace orcus;
using namespace std;

void test_base64_text_input(const char* p)
{
    cout << "input: '" << p << "'" << endl;
    size_t n = strlen(p);
    vector<char> input(p, p+n);
    string encoded;
    encode_to_base64(input, encoded);
    cout << "encoded: '" << encoded << "'" << endl;

    vector<char> decoded;
    decode_from_base64(&encoded[0], encoded.size(), decoded);
    cout << "decoded: '";
    vector<char>::const_iterator it = decoded.begin(), it_end = decoded.end();
    for (; it != it_end; ++it)
        cout << *it;
    cout << "'" << endl;

    assert(input == decoded);
}

int main()
{
    test_base64_text_input("Hello there");
    test_base64_text_input("World domination!!!");
    test_base64_text_input("World domination!!");
    test_base64_text_input("World domination!");
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
