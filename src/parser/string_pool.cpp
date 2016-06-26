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
#include <unordered_set>
#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>

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
    vector<const string*>& m_store;
public:
    pstring_back_inserter(vector<const string*>& store) : m_store(store) {}
    void operator() (const unique_ptr<string>& p) { m_store.push_back(p.get()); }
};

struct pstring_less
{
    bool operator() (const string* p1, const string* p2) const { return m_less(*p1, *p2); }
private:
    less<string> m_less;
};

}

namespace orcus {

typedef std::unordered_set<pstring, pstring::hash> string_set_type;
typedef std::vector<std::unique_ptr<std::string>> string_store_type;

struct string_pool::impl
{
    string_set_type m_set;
    string_store_type m_store;
    /** duplicate string instances */
    string_store_type m_merged_store;
};

string_pool::string_pool() : mp_impl(orcus::make_unique<impl>()) {}

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

    string_set_type::const_iterator itr = mp_impl->m_set.find(pstring(str, n));
    if (itr == mp_impl->m_set.end())
    {
        // This string has not been interned.  Intern it.
        mp_impl->m_store.push_back(orcus::make_unique<string>(str, n));
        pair<string_set_type::iterator,bool> r = mp_impl->m_set.insert(pstring(mp_impl->m_store.back()->data(), n));
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
    cout << "interned string count: " << mp_impl->m_store.size() << endl;

    // Sort stored strings first.
    vector<const string*> sorted;
    sorted.reserve(mp_impl->m_store.size());
    for_each(mp_impl->m_store.begin(), mp_impl->m_store.end(), pstring_back_inserter(sorted));
    sort(sorted.begin(), sorted.end(), pstring_less());

    // Now dump them all to stdout.
    for_each(sorted.begin(), sorted.end(), dump_instance());
}

void string_pool::clear()
{
    mp_impl->m_set.clear();
    mp_impl->m_store.clear();
}

size_t string_pool::size() const
{
    return mp_impl->m_set.size();
}

void string_pool::swap(string_pool& other)
{
    std::swap(mp_impl, other.mp_impl);
}

void string_pool::merge(string_pool& other)
{
    string_store_type* other_store = &other.mp_impl->m_store;

    std::for_each(other_store->begin(), other_store->end(),
        [&](string_store_type::value_type& value)
        {
            const std::string* p = value.get();
            size_t n = p->size();

            pstring key(p->data(), n);
            string_set_type::const_iterator it = mp_impl->m_set.find(key);

            if (it == mp_impl->m_set.end())
            {
                // This is a new string value in this pool.  Move this string
                // instance in as-is.
                mp_impl->m_store.push_back(std::move(value));
                auto r = mp_impl->m_set.insert(key);
                if (!r.second)
                    throw general_error("failed to intern a new string instance.");
            }
            else
            {
                // This is a duplicate string value in this pool.  Move this
                // string instance in to the merged store.
                mp_impl->m_merged_store.push_back(std::move(value));
            }
        }
    );

    // Move all duplicate string values from the other store as-is.
    other_store = &other.mp_impl->m_merged_store;
    std::for_each(other_store->begin(), other_store->end(),
        [&](string_store_type::value_type& value)
        {
            mp_impl->m_merged_store.push_back(std::move(value));
        }
    );

    other.mp_impl->m_store.clear();
    other.mp_impl->m_merged_store.clear();
    other.mp_impl->m_set.clear();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
