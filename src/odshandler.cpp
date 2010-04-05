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

#include "odshandler.hpp"
#include "tokens.hpp"
#include "odscontext.hpp"
#include "global.hpp"

#include <iostream>

using namespace std;

namespace orcus {

ods_content_xml_handler::ods_content_xml_handler()
{
    m_context_stack.push_back(new ods_content_xml_context);
}

ods_content_xml_handler::~ods_content_xml_handler()
{
}

void ods_content_xml_handler::start_document()
{
    get_current_context().start_context();
}

void ods_content_xml_handler::end_document()
{
    get_current_context().end_context();
}

void ods_content_xml_handler::start_element(
    xmlns_token_t ns, xml_token_t name, const vector<xml_attr>& attrs)
{
    get_current_context().start_element(ns, name, attrs);
}

void ods_content_xml_handler::end_element(xmlns_token_t ns, xml_token_t name)
{
    get_current_context().end_element(ns, name);
}

void ods_content_xml_handler::characters(const char* ch, size_t len)
{
    get_current_context().characters(ch, len);
}

void ods_content_xml_handler::print_html(const string& filepath)
{
    if (m_context_stack.empty())
        return;

    static_cast<ods_content_xml_context&>(m_context_stack.front()).print_html(filepath);
}

ods_context_base& ods_content_xml_handler::get_current_context()
{
    return m_context_stack.back();
}

}
