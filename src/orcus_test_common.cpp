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

#include "orcus/pstring.hpp"

#include <cstdlib>
#include <cassert>
#include <string>

using namespace std;
using namespace orcus;

void test_common_pstring()
{
    {
        // test for trimming.
        string s1("test"), s2("  test"), s3("   test  ");
        pstring ps1(s1.c_str()), ps2(s2.c_str()), ps3(s3.c_str());
        assert(ps1 != ps2);
        assert(ps1 != ps3);
        assert(ps2 != ps3);

        pstring trimmed = ps1.trim();
        assert(ps1 == trimmed); // nothing to trim.
        assert(ps1 == ps2.trim());
        assert(ps1 == ps3.trim());
        assert(ps1.size() == ps2.trim().size());
        assert(ps1.size() == ps3.trim().size());
    }
}

int main()
{
    test_common_pstring();
    return EXIT_SUCCESS;
}
