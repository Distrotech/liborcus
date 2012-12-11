/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#include "xml_stream_parser.hpp"
#include "xml_stream_handler.hpp"

#include "orcus/tokens.hpp"
#include "orcus/sax_token_parser.hpp"

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

namespace orcus {

// ============================================================================

xml_stream_parser::parse_error::parse_error(const string& msg) :
    m_msg(msg) {}

xml_stream_parser::parse_error::~parse_error() throw() {}

const char* xml_stream_parser::parse_error::what() const throw()
{
    return m_msg.c_str();
}

xml_stream_parser::xml_stream_parser(const tokens& tokens, const char* content, size_t size, const string& name) :
    m_tokens(tokens), mp_handler(NULL), m_content(content), m_size(size), m_name(name)
{
}

xml_stream_parser::~xml_stream_parser()
{
}

void xml_stream_parser::parse()
{
    if (!mp_handler)
        return;

    sax_token_parser<xml_stream_handler, tokens> sax(m_content, m_size, m_tokens, *mp_handler);
    sax.parse();
}

void xml_stream_parser::set_handler(xml_stream_handler* handler)
{
    mp_handler = handler;
}

xml_stream_handler* xml_stream_parser::get_handler() const
{
    return mp_handler;
}

}
