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

#include "orcus/pstring.hpp"

#include <cassert>
#include <iostream>
#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/unordered_set.hpp>
#include <boost/functional/hash.hpp>

using namespace std;

namespace orcus {

namespace {

struct pstring_hash
{
    size_t operator() (const string* p) const { return m_hash(*p); }
private:
    boost::hash<string> m_hash;
};

struct pstring_equal_to
{
    bool operator() (const string* p1, const string* p2) const { return m_equal_to(*p1, *p2); }
private:
    equal_to<string> m_equal_to;
};

struct pstring_less
{
    bool operator() (const string* p1, const string* p2) const { return m_less(*p1, *p2); }
private:
    less<string> m_less;
};

struct delete_instance : public unary_function<void, string*>
{
    void operator() (string* p) { delete p; }
};

struct dump_instance : public unary_function<void, string*>
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
    void operator() (const string* p) { m_store.push_back(p); }
private:
    vector<const string*>& m_store;
};

typedef boost::unordered_set<string*, pstring_hash, pstring_equal_to> pstring_store_type;

/**
 * Internal cache to store interned string instances.
 */
struct _interned_strings {
    pstring_store_type store;
    ::boost::mutex mtx;
} interned_strings;

}

pstring pstring::intern(const char* str)
{
    return intern(str, strlen(str));
}

pstring pstring::intern(const char* str, size_t n)
{
    if (!n)
        return pstring();

    ::boost::mutex::scoped_lock(interned_strings.mtx);

    string* new_str = new string(str, n);
    pstring_store_type::const_iterator itr = interned_strings.store.find(new_str);
    if (itr == interned_strings.store.end())
    {
        // This string has not been interned.  Intern it.
        interned_strings.store.insert(new_str);
        assert(new_str->size() == n);
        return pstring(&(*new_str)[0], n);
    }

    // This string has already been interned.
    delete new_str;

    const string* stored_str = *itr;
    assert(stored_str->size() == n);
    return pstring(&(*stored_str)[0], n);
}

void pstring::intern::dispose()
{
    ::boost::mutex::scoped_lock(interned_strings.mtx);
    for_each(interned_strings.store.begin(), interned_strings.store.end(), delete_instance());
    interned_strings.store.clear();
}

size_t pstring::intern::size()
{
    ::boost::mutex::scoped_lock(interned_strings.mtx);
    return interned_strings.store.size();
}

void pstring::intern::dump()
{
    ::boost::mutex::scoped_lock(interned_strings.mtx);
    cout << "interned string count: " << interned_strings.store.size() << endl;

    // Sorted stored strings first.
    vector<const string*> sorted;
    sorted.reserve(interned_strings.store.size());
    for_each(interned_strings.store.begin(), interned_strings.store.end(), pstring_back_inserter(sorted));
    sort(sorted.begin(), sorted.end(), pstring_less());

    // Now dump them all to stdout.
    for_each(sorted.begin(), sorted.end(), dump_instance());
}

size_t pstring::hash::operator() (const pstring& val) const
{
    // TODO: make this hashing algoritm more efficient.
    size_t hash_val = val.size();
    size_t loop_size = ::std::min<size_t>(hash_val, 20); // prevent too much looping.
    const char* p = val.get();
    for (size_t i = 0; i < loop_size; ++i, ++p)
    {
        hash_val += static_cast<size_t>(*p);
        hash_val *= 2;
    }

    return hash_val;
}

bool pstring::operator== (const pstring& r) const
{
    if (m_size != r.m_size)
        // lengths differ.
        return false;

    const char* pos1 = m_pos;
    const char* pos2 = r.m_pos;
    for (size_t i = 0; i < m_size; ++i, ++pos1, ++pos2)
        if (*pos1 != *pos2)
            return false;

    return true;
}

bool pstring::operator< (const pstring& r) const
{
    size_t n = std::min(m_size, r.m_size);
    const char* p1 = m_pos;
    const char* p2 = r.m_pos;
    for (size_t i = 0; i < n; ++i, ++p1, ++p2)
    {
        if (*p1 == *p2)
            continue;

        return *p1 < *p2;
    }

    return m_size < r.m_size;
}

bool pstring::operator== (const char* str) const
{
    size_t n = ::std::strlen(str);
    if (n != m_size)
        // lengths differ.
        return false;

    if (!m_size)
        // both are empty strings.
        return true;

    return ::std::strncmp(str, m_pos, n) == 0;
}

pstring pstring::trim() const
{
    const char* p = m_pos;
    const char* p_end = p + m_size;
    // Find the first non-space character.
    for ( ;p != p_end; ++p)
    {
        switch (*p)
        {
            case ' ':
            case 0x0A:
            case 0x0D:
                continue;
            default:
                ;
        }
        break;
    }

    if (p == p_end)
    {
        // This string is empty.
        return pstring();
    }

    // Find the last non-space character.
    for (--p_end; p_end != p; --p_end)
    {
        switch (*p_end)
        {
            case ' ':
            case 0x0A:
            case 0x0D:
                continue;
            default:
                ;
        }
        break;
    }

    ++p_end;
    return pstring(p, p_end-p);
}

pstring pstring::intern() const
{
    return intern(m_pos, m_size);
}

}
