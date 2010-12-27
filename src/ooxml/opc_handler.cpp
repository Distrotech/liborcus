/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#include "ooxml/opc_handler.hpp"
#include "ooxml/opc_context.hpp"
#include "tokens.hpp"

#include <iostream>

using namespace std;

namespace orcus {

opc_content_types_handler::opc_content_types_handler(const tokens& _tokens) :
    mp_context(new opc_content_types_context(_tokens))
{
}

opc_content_types_handler::~opc_content_types_handler()
{
    delete mp_context;
}

void opc_content_types_handler::start_document()
{
}

void opc_content_types_handler::end_document()
{
}

void opc_content_types_handler::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t &attrs)
{
    mp_context->start_element(ns, name, attrs);
}

void opc_content_types_handler::end_element(xmlns_token_t ns, xml_token_t name)
{
    mp_context->end_element(ns, name);
}

void opc_content_types_handler::characters(const pstring &str)
{
    mp_context->characters(str);
}

void opc_content_types_handler::pop_parts(vector<xml_part_t>& parts)
{
    if (!mp_context)
        return;

    mp_context->pop_parts(parts);
}

void opc_content_types_handler::pop_ext_defaluts(vector<xml_part_t>& ext_defaults)
{
    if (!mp_context)
        return;

    mp_context->pop_ext_defaults(ext_defaults);
}

}
