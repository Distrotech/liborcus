/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_PSTRING_HPP__
#define __ORCUS_PSTRING_HPP__

#include "orcus/env.hpp"

#include <cstdlib>
#include <string>
#include <cstring>
#include <ostream>

namespace orcus {

class string_pool;

/**
 * This string class does not store any char arrays, but it only stores the
 * position of the first char in the memory, and the size of the char array.
 */
class ORCUS_DLLPUBLIC pstring
{
    friend ::std::ostream& operator<< (::std::ostream& os, const pstring& str);

public:

    pstring() : m_pos(NULL), m_size(0) {}
    pstring(const char* _pos) : m_pos(_pos) { m_size = std::strlen(_pos); }
    pstring(const char* _pos, size_t _size) : m_pos(_pos), m_size(_size) {}

    ::std::string str() const { return ::std::string(m_pos, m_size); }

    size_t size() const { return m_size; }
    const char& operator[](size_t idx) const { return m_pos[idx]; }

    pstring& operator= (const pstring& r)
    {
        m_pos = r.m_pos;
        m_size = r.m_size;
        return *this;
    }

    const char* get() const { return m_pos; }

    bool operator== (const pstring& r) const;

    bool operator!= (const pstring& r) const
    {
        return !operator==(r);
    }

    bool operator< (const pstring& r) const;

    bool operator== (const char* _str) const;

    bool operator!= (const char* _str) const
    {
        return !operator==(_str);
    }

    pstring trim() const;

    bool empty() const { return m_size == 0; }

    void clear()
    {
        m_pos = NULL;
        m_size = 0;
    }

    struct ORCUS_DLLPUBLIC hash
    {
        size_t operator() (const pstring& val) const;
    };

private:
    const char* m_pos;
    size_t      m_size;
};

inline ::std::ostream& operator<< (::std::ostream& os, const pstring& str)
{
    return os << str.str();
}

ORCUS_DLLPUBLIC std::string operator+ (const std::string& left, const pstring& right);
ORCUS_DLLPUBLIC std::string& operator+= (std::string& left, const pstring& right);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
