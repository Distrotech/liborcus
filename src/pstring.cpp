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

#include <cassert>
#include <unordered_set>

#include <boost/thread/mutex.hpp>

using namespace std;

namespace orcus {

namespace {

struct pstring_hash
{
    size_t operator() (const string* p) const { return m_hash(*p); }
private:
    hash<string> m_hash;
};

struct pstring_equal_to
{
    bool operator() (const string* p1, const string* p2) const { return m_equal_to(*p1, *p2); }
private:
    equal_to<string> m_equal_to;
};

struct delete_instance : public unary_function<void, string*>
{
    void operator() (string* p) { delete p; }
};

typedef unordered_set<string*, pstring_hash, pstring_equal_to> pstring_store_type;

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

pstring pstring::intern() const
{
    return intern(m_pos, m_size);
}

}
