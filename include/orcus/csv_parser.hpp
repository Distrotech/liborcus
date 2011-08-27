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

#ifndef __ORCUS_CSV_PARSER_HPP__
#define __ORCUS_CSV_PARSER_HPP__

#define ORCUS_DEBUG_CSV 1

#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <cassert>
#include <sstream>

#if ORCUS_DEBUG_CSV
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus {

struct csv_parser_config
{
    std::string delimiters;
};

class csv_parse_error : public std::exception
{
    std::string m_msg;
public:
    csv_parse_error(const std::string& msg) : m_msg(msg) {}
    virtual ~csv_parse_error() throw() {}
    virtual const char* what() const throw() { return m_msg.c_str(); }
};

template<typename _Handler>
class csv_parser
{
public:
    typedef _Handler handler_type;

    csv_parser(const char* p, size_t n, handler_type& hdl, const csv_parser_config& options);
    void parse();

private:
    bool has_char() const { return m_pos < m_length; }
    void next();
    char cur_char() const;

    bool is_delim(char c) const;

    // handlers
    void row();
    void cell();

private:
    handler_type& m_handler;
    const csv_parser_config& m_config;
    const char* mp_char;
    size_t m_pos;
    size_t m_length;
};

template<typename _Handler>
csv_parser<_Handler>::csv_parser(const char* p, size_t n, handler_type& hdl, const csv_parser_config& options) :
    m_handler(hdl), m_config(options), mp_char(p), m_pos(0), m_length(n) {}

template<typename _Handler>
void csv_parser<_Handler>::parse()
{
#if ORCUS_DEBUG_CSV
    const char* p = mp_char;
    for (size_t i = m_pos; i < m_length; ++i, ++p)
        std::cout << *p;
    std::cout << std::endl;
#endif

    m_handler.begin_parse();
    while (has_char())
        row();
    m_handler.end_parse();
}

template<typename _Handler>
void csv_parser<_Handler>::next()
{
    ++m_pos;
    ++mp_char;
}

template<typename _Handler>
char csv_parser<_Handler>::cur_char() const
{
    return *mp_char;
}

template<typename _Handler>
bool csv_parser<_Handler>::is_delim(char c) const
{
    return m_config.delimiters.find(c) != std::string::npos;
}

template<typename _Handler>
void csv_parser<_Handler>::row()
{
    m_handler.begin_row();
    while (true)
    {
        cell();
        if (!has_char())
        {
            m_handler.end_row();
            return;
        }

        char c = cur_char();
        if (c == '\n')
        {
            next();
#if ORCUS_DEBUG_CSV
            cout << "(LF)" << endl;
#endif
            m_handler.end_row();
            return;
        }

        assert(is_delim(c));
        next();
    }
}

template<typename _Handler>
void csv_parser<_Handler>::cell()
{
    const char* p = mp_char;
    size_t len = 0;
    char c = cur_char();
    while (c != '\n' && !is_delim(c))
    {
        ++len;
        next();
        if (!has_char())
            break;
        c = cur_char();
    }

    if (!len)
        p = NULL;

    m_handler.cell(p, len);
#if ORCUS_DEBUG_CSV
    cout << "(cell:'" << std::string(p, len) << "')";
#endif
}

}

#endif
