/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef CSV_PARSER_BASE_HPP
#define CSV_PARSER_BASE_HPP

#include "env.hpp"
#include "cell_buffer.hpp"
#include "parser_global.hpp"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <cassert>
#include <sstream>

#define ORCUS_DEBUG_CSV 0

#if ORCUS_DEBUG_CSV
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus { namespace csv {

struct ORCUS_DLLPUBLIC parser_config
{
    std::string delimiters;
    char text_qualifier;
    bool trim_cell_value:1;

    parser_config();
};

class ORCUS_DLLPUBLIC parse_error : public std::exception
{
    std::string m_msg;
public:
    parse_error(const std::string& msg);
    virtual ~parse_error() throw();
    virtual const char* what() const throw();
};

class ORCUS_DLLPUBLIC parser_base
{
protected:
    const csv::parser_config& m_config;
    cell_buffer m_cell_buf;
    const char* mp_char;
    size_t m_pos;
    size_t m_length;

protected:
    parser_base(const char* p, size_t n, const parser_config& config);

    bool has_char() const { return m_pos < m_length; }
    bool has_next() const { return m_pos + 1 < m_length; }
    void next();
    char cur_char() const;
    char next_char() const;

    /**
     * This is different from the global 'is_blank' in that it doesn't treat
     * linefeed and carriage return characters as non-blanks.
     */
    bool is_blank(char c) const;
    bool is_delim(char c) const;
    bool is_text_qualifier(char c) const;

    void skip_blanks();
};

}}

#endif
