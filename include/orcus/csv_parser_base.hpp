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

}}

#endif
