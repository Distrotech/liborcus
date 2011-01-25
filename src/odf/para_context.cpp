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

#include "orcus/odf/para_context.hpp"
#include "orcus/odf/odf_token_constants.hpp"
#include "orcus/model/interface.hpp"

#include <iostream>

using namespace std;

namespace orcus {

text_para_context::text_para_context(const tokens& tokens, model::shared_strings_base* ssb) :
    xml_context_base(tokens),
    mp_sstrings(ssb),
    m_string_index(0)
{
}

text_para_context::~text_para_context()
{
}

bool text_para_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* text_para_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void text_para_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
    // not implemented yet.
}

void text_para_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == XMLNS_text && name == XML_span)
    {
        // text span.
    }
}

bool text_para_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_text && name == XML_span)
    {
        // text span.
    }

    m_string_index = mp_sstrings->add(m_current_content.get(), m_current_content.size());
    return pop_stack(ns, name);
}

void text_para_context::characters(const pstring& str)
{
    m_current_content = str;
}

size_t text_para_context::get_string_index() const
{
    return m_string_index;
}

const pstring& text_para_context::get_content() const
{
    return m_current_content;
}

}
