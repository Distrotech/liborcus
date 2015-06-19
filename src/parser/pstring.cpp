/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/pstring.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/parser_global.hpp"

#include <cassert>
#include <iostream>
#include <vector>

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

std::string operator+ (const std::string& left, const pstring& right)
{
    std::string ret = left;
    if (!right.empty())
    {
        const char* p = right.get();
        const char* p_end = p + right.size();
        for (; p != p_end; ++p)
            ret.push_back(*p);
    }
    return ret;
}

std::string& operator+= (std::string& left, const pstring& right)
{
    if (!right.empty())
    {
        const char* p = right.get();
        const char* p_end = p + right.size();
        for (; p != p_end; ++p)
            left.push_back(*p);
    }
    return left;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
