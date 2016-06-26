/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/string_pool.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

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

void test_merge()
{
    string_pool pool1;
    std::unique_ptr<string_pool> pool2(orcus::make_unique<string_pool>());

    pool1.intern("A");
    pool1.intern("B");
    pool1.intern("C");
    pstring v1 = pool1.intern("same value").first;

    pool2->intern("D");
    pool2->intern("E");
    pool2->intern("F");
    pstring v2 = pool2->intern("same value").first;

    assert(pool1.size() == 4);
    assert(pool2->size() == 4);

    pool1.merge(*pool2);

    assert(pool1.size() == 7);
    assert(pool2->size() == 0);

    pool2.reset(); // Delete the pool2 instance altogether.

    // This should not create a new entry.
    auto r = pool1.intern("F");
    assert(!r.second);

    // v2 still points to the original string in pool2, which should now be in
    // the merged store in pool1 (thus valid).
    assert(v1 == v2);
}

int main()
{
    test_basic();
    test_merge();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
