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

#include "xml_simple_stream_handler.hpp"
#include "orcus/xml_context.hpp"

#include <cassert>

namespace orcus {

xml_simple_stream_handler::xml_simple_stream_handler(xml_context_base* context) :
    xml_stream_handler(),
    mp_context(context)
{
    assert(mp_context);
}

xml_simple_stream_handler::~xml_simple_stream_handler()
{
    delete mp_context;
}

xml_context_base& xml_simple_stream_handler::get_context()
{
    return *mp_context;
}

void xml_simple_stream_handler::start_document()
{
}

void xml_simple_stream_handler::end_document()
{
}

void xml_simple_stream_handler::start_element(const sax_token_parser_element& elem)
{
    mp_context->start_element(elem.ns, elem.name, elem.attrs);
}

void xml_simple_stream_handler::end_element(const sax_token_parser_element& elem)
{
    mp_context->end_element(elem.ns, elem.name);
}

void xml_simple_stream_handler::characters(const pstring& str)
{
    mp_context->characters(str);
}

}
