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

#include "orcus/csv_parser_base.hpp"

namespace orcus { namespace csv {

parser_config::parser_config() :
    text_qualifier('\0'),
    trim_cell_value(false) {}

parse_error::parse_error(const std::string& msg) : m_msg(msg) {}

parse_error::~parse_error() throw() {}

const char* parse_error::what() const throw()
{
    return m_msg.c_str();
}

parser_base::parser_base(
    const char* p, size_t n, const csv::parser_config& config) :
    m_config(config), mp_char(p), m_pos(0), m_length(n) {}

void parser_base::next()
{
    ++m_pos;
    ++mp_char;
}

char parser_base::cur_char() const
{
    return *mp_char;
}

char parser_base::next_char() const
{
    return *(mp_char+1);
}

bool parser_base::is_blank(char c) const
{
    if (c == ' ')
        return true;

    if (c == '\t')
        // tab character.
        return true;

    return false;
}

bool parser_base::is_delim(char c) const
{
    return m_config.delimiters.find(c) != std::string::npos;
}

bool parser_base::is_text_qualifier(char c) const
{
    return m_config.text_qualifier == c;
}

void parser_base::skip_blanks()
{
    for (; has_char(); next())
    {
        if (!is_blank(*mp_char))
            break;
    }
}


}}
