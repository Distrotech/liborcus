/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/string_pool.hpp"
#include "orcus/pstring.hpp"

#include <iostream>
#include <cassert>

using namespace std;
using namespace orcus;

void test_basic()
{
    const char* static_text = "test";

    string_pool pool;
    assert(pool.size() == 0);

    pair<pstring, bool> ret = pool.intern("foo");
    assert(ret.first == "foo");
    assert(ret.second); // new instance

    ret = pool.intern("foo");
    assert(ret.first == "foo");
    assert(!ret.second); // existing instance.

    // Empty strings should not be interned.
    ret = pool.intern("");
    assert(ret.first.empty());
    assert(!ret.second);

    ret = pool.intern("A");
    cout << "interned string: " << ret.first << endl;
    assert(ret.second);
    assert(pool.size() == 2);

    // Duplicate string.
    ret = pool.intern("A");
    cout << "interned string: " << ret.first << endl;
    assert(!ret.second);

    ret = pool.intern("B");
    cout << "interned string: " << ret.first << endl;
    assert(pool.size() == 3);

    // Interning an already-intern string should return a pstring with
    // identical memory address.
    pstring str = ret.first;
    pstring str2 = pool.intern(str).first;
    assert(str == str2);
    assert(pool.size() == 3);
    assert(str.get() == str2.get()); // their memory address should be identical.

    pstring static_str(static_text);
    ret = pool.intern(static_str);
    str = ret.first;
    cout << "interned string: " << str << endl;
    assert(pool.size() == 4);
    assert(str == static_str);
    assert(str.get() != static_str.get());
}

int main()
{
    test_basic();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
