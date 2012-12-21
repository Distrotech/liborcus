/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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
