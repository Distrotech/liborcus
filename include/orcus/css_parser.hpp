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

#ifndef __ORCUS_CSS_PARSER_HPP__
#define __ORCUS_CSS_PARSER_HPP__

#define ORCUS_DEBUG_CSS 1

#include <cstdlib>
#include <cstring>
#if ORCUS_DEBUG_CSS
#include <iostream>
#include <string>
#endif

namespace orcus {

template<typename _Handler>
class css_parser
{
public:
    typedef _Handler handler_type;

    css_parser(const char* p, size_t n);
    void parse();

private:
    void shrink_stream();
    void next();

    size_t remaining_size() const { return m_length - m_pos - 1; }
    bool has_char() const { return m_pos < m_length; }
    bool is_blank(char c) const
    {
        return c == ' ' || c == '\t' || c == '\n';
    }

    const char* mp_char;
    size_t m_pos;
    size_t m_length;
};

template<typename _Handler>
css_parser<_Handler>::css_parser(const char* p, size_t n) :
    mp_char(p), m_pos(0), m_length(n) {}

template<typename _Handler>
void css_parser<_Handler>::parse()
{
    shrink_stream();
    std::cout << "'";
    while (has_char())
    {
        std::cout << *mp_char;
        next();
    }
    std::cout << "'" << std::endl;
}

template<typename _Handler>
void css_parser<_Handler>::shrink_stream()
{
    // Skip any leading blanks.
    while (has_char())
    {
        if (!is_blank(*mp_char))
            break;
        next();
    }

    size_t n = remaining_size();
    if (!n)
        return;

    // Skip any trailing blanks.
    const char* p = mp_char + n;
    for (; p != mp_char; --p, --m_length)
    {
        if (!is_blank(*p))
            break;
    }

    // Skip leading <!-- if present.

    const char* com_open = "<!--";
    size_t com_open_len = std::strlen(com_open);
    if (remaining_size() < com_open_len)
        // Not enough stream left.  Bail out.
        return;

    p = mp_char;
    for (size_t i = 0; i < com_open_len; ++i, ++p)
    {
        if (*p != com_open[i])
            return;
        next();
    }
    mp_char = p;

    // Skip leading blanks once again.
    while (has_char())
    {
        if (!is_blank(*mp_char))
            break;
        next();
    }

    // Skip trailing --> if present.
    const char* com_close = "-->";
    size_t com_close_len = std::strlen(com_close);
    n = remaining_size();
    if (n < com_close_len)
        // Not enough stream left.  Bail out.
        return;

    p = mp_char + n; // move to the last char.
    for (size_t i = com_close_len; i > 0; --i, --p)
    {
        if (*p != com_close[i-1])
            return;
    }
    m_length -= com_close_len;

    // Skip trailing blanks once again.
    for (; p != mp_char; --p, --m_length)
    {
        if (!is_blank(*p))
            break;
    }
}

template<typename _Handler>
void css_parser<_Handler>::next()
{
    ++m_pos;
    ++mp_char;
}

}

#endif
