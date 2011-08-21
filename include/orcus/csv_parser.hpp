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
#endif

namespace orcus {

struct csv_parser_options
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

    csv_parser(const char* p, size_t n, handler_type& hdl, const csv_parser_options& options);
    void parse();

private:
    handler_type& m_handler;
    const csv_parser_options& m_options;
    const char* mp_char;
    size_t m_pos;
    size_t m_length;
};

template<typename _Handler>
csv_parser<_Handler>::csv_parser(const char* p, size_t n, handler_type& hdl, const csv_parser_options& options) :
    m_handler(hdl), m_options(options), mp_char(p), m_pos(0), m_length(n) {}

template<typename _Handler>
void csv_parser<_Handler>::parse()
{
#if ORCUS_DEBUG_CSV
    const char* p = mp_char;
    for (size_t i = m_pos; i < m_length; ++i, ++p)
        std::cout << *p;
    std::cout << std::endl;
#endif
}

}

#endif
