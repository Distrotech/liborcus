/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/string_pool.hpp"

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

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
