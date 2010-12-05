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

#ifndef __ORCUS_PSTRING_HPP__
#define __ORCUS_PSTRING_HPP__

#include <cstdlib>
#include <string>
#include <cstring>

namespace orcus {

/**
 * This string class does not store any char arrays, but it only stores the 
 * position of the first char in the memory, and the size of the char array.
 */
class pstring
{
    friend ::std::ostream& operator<< (::std::ostream& os, const pstring& str);

public:
    pstring() : m_pos(NULL), m_size(0) {}
    pstring(const char* pos) : m_pos(pos) { m_size = ::std::strlen(pos); }
    pstring(const char* pos, size_t size) : m_pos(pos), m_size(size) {}

    ::std::string str() const { return ::std::string(m_pos, m_size); }

    size_t size() const { return m_size; }
    char operator[](size_t idx) const { return m_pos[idx]; }

    pstring& operator= (const pstring& r)
    {
        m_pos = r.m_pos;
        m_size = r.m_size;
        return *this;
    }

    const char* get() const { return m_pos; }

    bool operator== (const pstring& r) const
    {
        if (m_size != r.m_size)
            return false;

        for (size_t i = 0; i < m_size; ++i)
            if (m_pos[i] != r.m_pos[i])
                return false;

        return true;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    void clear()
    {
        m_pos = NULL;
        m_size = 0;
    }

private:
    const char* m_pos;
    size_t      m_size;
};

inline ::std::ostream& operator<< (::std::ostream& os, const pstring& str)
{
    return os << str.str();
}

}

#endif
