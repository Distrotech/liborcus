/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#include "orcus/model/shared_strings.hpp"
#include "orcus/pstring.hpp"

#include <iostream>
#include <algorithm>

using namespace std;

namespace orcus { namespace model {

shared_strings::shared_strings()
{
}

shared_strings::~shared_strings()
{
}

size_t shared_strings::append(const char* s, size_t n)
{
    pstring ps = pstring(s, n).intern();
    return append_to_pool(ps);
}

size_t shared_strings::add(const char* s, size_t n)
{
    pstring ps = pstring(s, n).intern();

    // Check if this string is already in the pool.
    str_index_map_type::const_iterator itr = m_set.find(ps);
    if (itr != m_set.end())
    {
        // It's already in the pool.
        return itr->second;
    }

    // Not in the pool yet.  Insert it into the pool.
    return append_to_pool(ps);
}

size_t shared_strings::append_to_pool(const pstring& ps)
{
    size_t index = m_strings.size() - 1;
    m_strings.push_back(ps);
    m_set.insert(str_index_map_type::value_type(ps, index));
    return index;
}

bool shared_strings::has(size_t index) const
{
    return index < m_strings.size();
}

const pstring& shared_strings::get(size_t index) const
{
    return m_strings[index];
}

namespace {

struct print_string : public unary_function<void, pstring>
{
    size_t m_count;
public:
    print_string() : m_count(1) {}
    void operator() (const pstring& ps)
    {
        cout << m_count++ << ": '" << ps << "'" << endl;
    }
};

}

void shared_strings::dump() const
{
    cout << "number of shared strings: " << m_strings.size() << endl;
//  for_each(m_strings.begin(), m_strings.end(), print_string());
}

}}
