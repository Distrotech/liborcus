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

#ifndef __ORCUS_XML_STREAM_HANDLER_HPP__
#define __ORCUS_XML_STREAM_HANDLER_HPP__

#include "orcus/sax_token_parser.hpp"

#include <cstdlib>
#include <string>
#include <vector>

namespace orcus {

class xml_context_base;

class xml_stream_handler
{
    xml_context_base* mp_root_context;
    typedef std::vector<xml_context_base*> context_stack_type;
    context_stack_type m_context_stack;

    xml_stream_handler(); // disabled
public:
    xml_stream_handler(xml_context_base* root_context);
    virtual ~xml_stream_handler() = 0;

    virtual void start_document() = 0;
    virtual void end_document() = 0;

    virtual void start_element(const sax_token_parser_element& elem);
    virtual void end_element(const sax_token_parser_element& elem);
    virtual void characters(const pstring& str, bool transient);

protected:
    xml_context_base& get_current_context();

};

}

#endif
