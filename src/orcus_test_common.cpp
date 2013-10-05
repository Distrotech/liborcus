/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
