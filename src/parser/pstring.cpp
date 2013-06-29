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
#include "orcus/string_pool.hpp"
#include "orcus/parser_global.hpp"

#include <cassert>
#include <iostream>
#include <vector>

#include <boost/thread/mutex.hpp>

using namespace std;

namespace orcus {

size_t pstring::hash::operator() (const pstring& val) const
{
    size_t hash_val = 0;
    const char* p = val.get();
    const char* p_end = p + val.size();
    for (; p != p_end; ++p)
    {
        hash_val *= 0x01000193;
        hash_val ^= *p;
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

bool pstring::operator== (const char* _str) const
{
    size_t n = std::strlen(_str);
    if (n != m_size)
        // lengths differ.
        return false;

    if (!m_size)
        // both are empty strings.
        return true;

    return std::strncmp(_str, m_pos, n) == 0;
}

pstring pstring::trim() const
{
    const char* p = m_pos;
    const char* p_end = p + m_size;
    // Find the first non-space character.
    for ( ;p != p_end; ++p)
    {
        if (is_blank(*p))
            continue;
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
        if (is_blank(*p_end))
            continue;
        break;
    }

    ++p_end;
    return pstring(p, p_end-p);
}

}
