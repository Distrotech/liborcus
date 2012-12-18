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

#include "string_pool.hpp"

#include "orcus/global.hpp"
#include "orcus/pstring.hpp"
#include "orcus/exception.hpp"

#include <iostream>

using namespace std;

namespace {

struct dump_instance : public unary_function<string*, void>
{
    dump_instance() : counter(0) {}
    void operator() (const string* p)
    {
        cout << counter++ << ": '" << *p << "'" << endl;
    }
private:
    size_t counter;
};

class pstring_back_inserter
{
public:
    pstring_back_inserter(vector<const string*>& store) : m_store(store) {}
    void operator() (const string& r) { m_store.push_back(&r); }
private:
    vector<const string*>& m_store;
};

struct pstring_less
{
    bool operator() (const string* p1, const string* p2) const { return m_less(*p1, *p2); }
private:
    less<string> m_less;
};

}

namespace orcus {

size_t string_pool::string_hash::operator() (const string* p) const
{
    return m_hash(*p);
}

bool string_pool::string_equal_to::operator() (const string* p1, const string* p2) const
{
    return m_equal_to(*p1, *p2);
}

string_pool::string_pool() {}

string_pool::~string_pool()
{
    clear();
}

pair<pstring, bool> string_pool::intern(const char* str)
{
    return intern(str, strlen(str));
}

pair<pstring, bool> string_pool::intern(const char* str, size_t n)
{
    if (!n)
        return pair<pstring, bool>(pstring(), false);

    string_set_type::const_iterator itr = m_set.find(pstring(str, n));
    if (itr == m_set.end())
    {
        // This string has not been interned.  Intern it.
        m_store.push_back(new string(str, n));
        pair<string_set_type::iterator,bool> r = m_set.insert(pstring(&m_store.back()[0], n));
        if (!r.second)
            throw general_error("failed to intern a new string instance.");
        const pstring& ps = *r.first;
        assert(ps.size() == n);

        return pair<pstring, bool>(ps, true);
    }

    // This string has already been interned.

    const pstring& stored_str = *itr;
    assert(stored_str.size() == n);
    return pair<pstring, bool>(stored_str, false);
}

pair<pstring, bool> string_pool::intern(const pstring& str)
{
    return intern(str.get(), str.size());
}

void string_pool::dump() const
{
    cout << "interned string count: " << m_store.size() << endl;

    // Sorted stored strings first.
    vector<const string*> sorted;
    sorted.reserve(m_store.size());
    for_each(m_store.begin(), m_store.end(), pstring_back_inserter(sorted));
    sort(sorted.begin(), sorted.end(), pstring_less());

    // Now dump them all to stdout.
    for_each(sorted.begin(), sorted.end(), dump_instance());
}

void string_pool::clear()
{
    m_set.clear();
    m_store.clear();
}

size_t string_pool::size() const
{
    return m_store.size();
}

}

