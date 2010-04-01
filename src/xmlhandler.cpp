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

#include "xmlhandler.hpp"
#include "tokens.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xml_stream_handler::xml_stream_handler()
{
}

xml_stream_handler::~xml_stream_handler()
{
}

// ============================================================================

ods_content_xml_handler::ods_content_xml_handler()
{
}

ods_content_xml_handler::~ods_content_xml_handler()
{
}

void ods_content_xml_handler::start_document()
{
    cout << "start document" << endl;
}

void ods_content_xml_handler::end_document()
{
    cout << "end document" << endl;
}

void ods_content_xml_handler::start_element(
    xmlns_token_t ns, xml_token_t name, const vector<xml_attr>& attrs)
{
    cout << "<" << tokens::get_nstoken_name(ns) << ":" << tokens::get_token_name(name) << ">" << endl;
}

void ods_content_xml_handler::end_element(xmlns_token_t ns, xml_token_t name)
{
    cout << "</" << tokens::get_nstoken_name(ns) << ":" << tokens::get_token_name(name) << ">" << endl;
}

void ods_content_xml_handler::characters(const char* ch, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        cout << ch[i];
    cout << endl;
}

}
