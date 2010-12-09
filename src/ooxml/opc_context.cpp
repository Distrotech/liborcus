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

#include "ooxml/opc_context.hpp"

namespace orcus {

opc_content_types_context::opc_content_types_context(const tokens& _tokens) :
    xml_context_base(_tokens)
{
}

bool opc_content_types_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* opc_content_types_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void opc_content_types_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child)
{
}

void opc_content_types_context::start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_attr_t> &attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    warn_unhandled();
}

bool opc_content_types_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void opc_content_types_context::characters(const pstring &str)
{
}

}
