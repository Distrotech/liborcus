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
#include <exception>
#include <string>
#include <cassert>
#include <sstream>

#if ORCUS_DEBUG_CSS
#include <iostream>
#endif

namespace orcus {

class css_parse_error : public std::exception
{
    std::string m_msg;
public:
    css_parse_error(const std::string& msg) : m_msg(msg) {}
    virtual ~css_parse_error() throw() {}
    virtual const char* what() const throw() { return m_msg.c_str(); }
};

template<typename _Handler>
class css_parser
{
public:
    typedef _Handler handler_type;

    css_parser(const char* p, size_t n);
    void parse();

private:
    // Handlers - at the time a handler is called the current position is
    // expected to point to the first unprocessed non-blank character, and
    // each handler must set the current position to the next unprocessed
    // non-blank character when it finishes.
    void rule();
    void name();
    void property();
    void quoted_value();
    void value();
    void name_sep();
    void property_sep();
    void open_paren();

    void skip_blanks();
    void skip_blanks_reverse();
    void shrink_stream();
    void next();
    char cur_char() const;

    size_t remaining_size() const { return m_length - m_pos - 1; }
    bool has_char() const { return m_pos < m_length; }

    static bool is_blank(char c)
    {
        return c == ' ' || c == '\t' || c == '\n';
    }

    static bool is_alpha(char c)
    {
        if ('a' <= c && c <= 'z')
            return true;
        if ('A' <= c && c <= 'Z')
            return true;
        return false;
    }

    static bool is_name_char(char c)
    {
        switch (c)
        {
            case '-':
            case '.':
                return true;
        }

        return false;
    }

    static bool is_numeric(char c)
    {
        if ('0' <= c && c <= '9')
            return true;
        return false;
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

#if ORCUS_DEBUG_CSS
    std::cout << "compressed: '";
    const char* p = mp_char;
    for (size_t i = m_pos; i < m_length; ++i, ++p)
        std::cout << *p;
    std::cout << "'" << std::endl;
#endif
    for (; has_char(); next())
        rule();
}

template<typename _Handler>
void css_parser<_Handler>::rule()
{
    // <name> , ... , <name> { <properties> }
    while (has_char())
    {
        char c = cur_char();
        if (is_alpha(c) || c == '.')
        {
            name();
        }
        else if (c == ',')
        {
            name_sep();
        }
        else if (c == '{')
        {
            open_paren();
        }
        else
        {
            std::ostringstream os;
            os << "failed to parse '" << c << "'";
            throw css_parse_error(os.str());
        }
    }
}

template<typename _Handler>
void css_parser<_Handler>::name()
{
    assert(has_char());
    char c = cur_char();
    if (!is_alpha(c) && c != '.')
        throw css_parse_error("first character of a name must be an alphabet or a dot.");

    const char* p = mp_char;
    size_t len = 1;
    for (next(); has_char(); next())
    {
        c = cur_char();
        if (!is_alpha(c) && !is_name_char(c) && !is_numeric(c))
            break;
        ++len;
    }
    skip_blanks();

#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "name: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::property()
{
    // <name> : <value> , ... , <value>
    name();
    if (cur_char() != ':')
        throw css_parse_error("':' expected.");
    next();
    skip_blanks();
    while (has_char())
    {
        value();
        if (cur_char() != ',')
            break;
    }
    skip_blanks();
}

template<typename _Handler>
void css_parser<_Handler>::quoted_value()
{
}

template<typename _Handler>
void css_parser<_Handler>::value()
{
    assert(has_char());
    char c = cur_char();
    if (!is_alpha(c) && !is_numeric(c) && c != '-' && c != '+')
    {
        std::ostringstream os;
        os << "illegal first character of a value '" << c << "'";
        throw css_parse_error(os.str());
    }

    const char* p = mp_char;
    size_t len = 1;
    for (next(); has_char(); next())
    {
        c = cur_char();
        if (!is_alpha(c) && !is_name_char(c) && !is_numeric(c))
            break;
        ++len;
    }
    skip_blanks();

#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "value: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::name_sep()
{
    assert(cur_char() == ',');
#if ORCUS_DEBUG_CSS
    std::cout << "," << std::endl;
#endif
    next();
    skip_blanks();
}

template<typename _Handler>
void css_parser<_Handler>::property_sep()
{
#if ORCUS_DEBUG_CSS
    std::cout << ";" << std::endl;
#endif
    next();
    skip_blanks();
}

template<typename _Handler>
void css_parser<_Handler>::open_paren()
{
    // '{' <property> ';' ... ';' <property> '}'

    assert(cur_char() == '{');
#if ORCUS_DEBUG_CSS
    std::cout << "{" << std::endl;
#endif
    next();
    skip_blanks();

    // parse properties.
    while (has_char())
    {
        property();
        if (cur_char() != ';')
            break;
        property_sep();
        if (cur_char() == '}')
            // ';' immediately followed by '}'.  Do we allow this?
            break;
    }
    
    if (cur_char() != '}')
        throw css_parse_error("} expected.");
    next();
    skip_blanks();

#if ORCUS_DEBUG_CSS
    std::cout << "}" << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::skip_blanks()
{
    for (; has_char(); next())
    {
        if (!is_blank(*mp_char))
            break;
    }
}

template<typename _Handler>
void css_parser<_Handler>::skip_blanks_reverse()
{
    const char* p = mp_char + remaining_size();
    for (; p != mp_char; --p, --m_length)
    {
        if (!is_blank(*p))
            break;
    }
}

template<typename _Handler>
void css_parser<_Handler>::shrink_stream()
{
    // Skip any leading blanks.
    skip_blanks();

    if (!remaining_size())
        return;

    // Skip any trailing blanks.
    skip_blanks_reverse();

    // Skip leading <!-- if present.

    const char* com_open = "<!--";
    size_t com_open_len = std::strlen(com_open);
    if (remaining_size() < com_open_len)
        // Not enough stream left.  Bail out.
        return;

    const char* p = mp_char;
    for (size_t i = 0; i < com_open_len; ++i, ++p)
    {
        if (*p != com_open[i])
            return;
        next();
    }
    mp_char = p;

    // Skip leading blanks once again.
    skip_blanks();

    // Skip trailing --> if present.
    const char* com_close = "-->";
    size_t com_close_len = std::strlen(com_close);
    size_t n = remaining_size();
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

    skip_blanks_reverse();
}

template<typename _Handler>
void css_parser<_Handler>::next()
{
    ++m_pos;
    ++mp_char;
}

template<typename _Handler>
char css_parser<_Handler>::cur_char() const
{
    return *mp_char;
}

}

#endif
