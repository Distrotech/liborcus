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

ods_content_xml_handler::ods_content_xml_handler(ods_content_xml_context* context) :
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
    xml_token_pair_t parent = m_stack.empty() ? xml_token_pair_t(XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN) : m_stack.back();
    m_stack.push_back(xml_token_pair_t(ns, name));

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
                warn_unhandled(m_stack);
        }
    }
    else if (ns == XMLNS_table)
    {
        switch (name)
        {
            case XML_table:
                start_table(attrs, parent);
            break;
            case XML_table_column:
                start_table_column(attrs, parent);
            break;
            case XML_table_row:
                start_table_row(attrs, parent);
            break;
            case XML_table_cell:
                start_table_cell(attrs, parent);
            break;
            default:
                warn_unhandled(m_stack);
        }
    }
    else if (ns == XMLNS_text)
    {
        switch (name)
        {
            case XML_p:
                start_text_p(attrs, parent);
            break;
            default:
                warn_unhandled(m_stack);
        }
    }
    else
        warn_unhandled(m_stack);
}

void ods_content_xml_handler::end_element(xmlns_token_t ns, xml_token_t name)
{
    const xml_token_pair_t& r = m_stack.back();

    if (ns != r.first || name != r.second)
        throw general_error("mismatched element name");

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
                end_table();
            break;
            case XML_table_column:
                end_table_column();
            break;
            case XML_table_row:
                end_table_row();
            break;
            case XML_table_cell:
                end_table_cell();
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
                end_text_p();
            break;
            default:
                ;
        }
    }
}

void ods_content_xml_handler::characters(const char* ch, size_t len)
{
    if (!mp_context)
        return;
}

void ods_content_xml_handler::start_table(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (!mp_context)
        return;

    // parent(s) allowed: <office:spreadsheet>

    if (parent.first != XMLNS_office || parent.second != XML_spreadsheet)
    {
        warn_unexpected(m_stack);
        return;
    }

    mp_context->start_table(attrs);
}

void ods_content_xml_handler::end_table()
{
    if (!mp_context)
        return;

    mp_context->end_table();
}

void ods_content_xml_handler::start_table_column(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (!mp_context)
        return;

    if (parent.first == XMLNS_table)
    {
        switch (parent.second)
        {
            case XML_table:
                mp_context->start_column(attrs);
            break;
            default:
                warn_unexpected(m_stack);
        }
    }
    else
        warn_unexpected(m_stack);
}

void ods_content_xml_handler::end_table_column()
{
    if (!mp_context)
        return;

    mp_context->end_column();
}

void ods_content_xml_handler::start_table_row(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (!mp_context)
        return;

    if (parent.first == XMLNS_table)
    {
        switch (parent.second)
        {
            case XML_table:
                mp_context->start_row(attrs);
            break;
            default:
                warn_unexpected(m_stack);
        }
    }
    else
        warn_unexpected(m_stack);
}

void ods_content_xml_handler::end_table_row()
{
    if (!mp_context)
        return;

    mp_context->end_row();
}

void ods_content_xml_handler::start_table_cell(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (!mp_context)
        return;

    if (parent.first == XMLNS_table)
    {
        switch (parent.second)
        {
            case XML_table_row:
                mp_context->start_cell(attrs);
            break;
            default:
                warn_unexpected(m_stack);
        }
    }
    else
        warn_unexpected(m_stack);
}

void ods_content_xml_handler::end_table_cell()
{
    if (!mp_context)
        return;

    mp_context->end_cell();
}

void ods_content_xml_handler::start_text_p(const xml_attrs_t& attrs, const xml_token_pair_t& parent)
{
    if (!mp_context)
        return;

    if (parent.first == XMLNS_table)
    {
        switch (parent.second)
        {
            case XML_table_cell:
            break;
            default:
                warn_unexpected(m_stack);
        }
    }
    else
        warn_unexpected(m_stack);
}

void ods_content_xml_handler::end_text_p()
{
    if (!mp_context)
        return;
}

}
