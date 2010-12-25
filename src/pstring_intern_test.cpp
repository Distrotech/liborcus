/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include "pstring.hpp"

#include <string>
#include <iostream>
#include <cassert>

using namespace std;
using namespace orcus;

const char* static_text = "test";

int main()
{
    // Empty strings should not be interned.
    pstring str = pstring::intern("");
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(str.empty());
    assert(pstring::intern::size() == 0);

    str = pstring::intern("A");
    cout << "interned string: " << str << endl;
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(pstring::intern::size() == 1);

    // Duplicate string.
    str = pstring::intern("A");
    cout << "interned string: " << str << endl;
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(pstring::intern::size() == 1);

    str = pstring::intern("B");
    cout << "interned string: " << str << endl;
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(pstring::intern::size() == 2);

    // Interning an already-intern string should return a pstring with 
    // identical memory address.
    pstring str2 = str.intern();
    assert(str == str2);
    assert(pstring::intern::size() == 2);
    assert(str.get() == str2.get()); // their memory address should be identical.

    pstring static_str(static_text);
    str = static_str.intern();
    cout << "interned string: " << str << endl;
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(pstring::intern::size() == 3);
    assert(str == static_str);
    assert(str.get() != static_str.get());

    pstring::intern::dispose();
    cout << "dispose() called" << endl;
    cout << "intern size: " << pstring::intern::size() << endl;
    assert(pstring::intern::size() == 0);
    return EXIT_SUCCESS;
}
