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

#ifndef __ORCUS_CELL_BUFFER_HPP__
#define __ORCUS_CELL_BUFFER_HPP__

#include <string>

#define ORCUS_DEBUG_CELL_BUFFER 0

#if ORCUS_DEBUG_CELL_BUFFER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus {

/**
 * Temporary cell buffer used to convert cell values when needed.  This is
 * used in the sax and csv parsers.
 */
class cell_buffer
{
    std::string m_buffer;
    size_t m_buf_size; /// Logical buffer size. May differ from the actual buffer size.
public:
    cell_buffer() : m_buf_size(0) {}

    void append(const char* p, size_t len)
    {
        if (!len)
            return;

#if ORCUS_DEBUG_CELL_BUFFER
        cout << "cell_buffer::append: '" << std::string(p, len) << "'" << endl;
#endif

        size_t size_needed = m_buf_size + len;
        if (m_buffer.size() < size_needed)
            m_buffer.resize(size_needed);

        char* p_dest = &m_buffer[m_buf_size];
        std::strncpy(p_dest, p, len);
        m_buf_size += len;
    }

    void reset() { m_buf_size = 0; }
    const char* get() const { return &m_buffer[0]; }
    size_t size() const { return m_buf_size; }
};

}

#endif
