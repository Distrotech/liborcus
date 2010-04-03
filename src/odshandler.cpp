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

ods_content_xml_handler::ods_content_xml_handler(ods_context_base* context) :
    mp_context(context)
{
}

ods_content_xml_handler::~ods_content_xml_handler()
{
}

void ods_content_xml_handler::start_document()
{
    if (mp_context)
        mp_context->start_content();
}

void ods_content_xml_handler::end_document()
{
    if (mp_context)
        mp_context->end_content();
}

void ods_content_xml_handler::start_element(
    xmlns_token_t ns, xml_token_t name, const vector<xml_attr>& attrs)
{
    m_stack.push_back(pair<xmlns_token_t, xml_token_t>(ns, name));
    if (!mp_context)
        return;

    if (ns == XMLNS_office)
    {
        switch (name)
        {
            case XML_body:
            break;
            case XML_spreadsheet:
            break;
            default:
                ;
        }
    }
    else if (ns == XMLNS_table)
    {
        switch (name)
        {
            case XML_table:
                table_start(attrs);
            break;
            case XML_table_column:
            break;
            case XML_table_row:
            break;
            case XML_table_cell:
            break;
            default:
                ;
        }
    }
    else if (ns == XMLNS_text)
    {
        switch (name)
        {
            case XML_p:
            break;
            default:
                ;
        }
    }

//  cout << "<" << tokens::get_nstoken_name(ns) << ":" << tokens::get_token_name(name) << ">" << endl;
}

void ods_content_xml_handler::end_element(xmlns_token_t ns, xml_token_t name)
{
    const token_pair_type& r = m_stack.back();

    if (ns != r.first || name != r.second)
        throw general_error("mismatched element name");

//  cout << "</" << tokens::get_nstoken_name(ns) << ":" << tokens::get_token_name(name) << ">" << endl;
    m_stack.pop_back();
    if (!mp_context)
        return;

    if (ns == XMLNS_office)
    {
        switch (name)
        {
            case XML_body:
            break;
            case XML_spreadsheet:
            break;
            default:
                ;
        }
    }
    else if (ns == XMLNS_table)
    {
        switch (name)
        {
            case XML_table:
                table_end();
            break;
            case XML_table_column:
            break;
            case XML_table_row:
            break;
            case XML_table_cell:
            break;
            default:
                ;
        }
    }
    else if (ns == XMLNS_text)
    {
        switch (name)
        {
            case XML_p:
            break;
            default:
                ;
        }
    }
}

void ods_content_xml_handler::characters(const char* ch, size_t len)
{
//  for (size_t i = 0; i < len; ++i)
//      cout << ch[i];
//  cout << endl;
}

void ods_content_xml_handler::table_start(const xml_attrs_type& attrs)
{
    cout << "table start" << endl;
}

void ods_content_xml_handler::table_end()
{
    cout << "table end" << endl;
}

}
