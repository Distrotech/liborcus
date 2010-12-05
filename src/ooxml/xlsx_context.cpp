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

#include "ooxml/xlsx_context.hpp"
#include "global.hpp"
#include "tokens.hpp"
#include "ooxml/ooxml_token_constants.hpp"

#include <iostream>
#include <fstream>

using namespace std;

namespace orcus {

namespace {

/**
 * for worksheet attributes.
 */
class worksheet_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    explicit worksheet_attr_parser(xlsx_sheet_xml_context& parent) : m_parent(parent) {}

    void operator() (const xml_attr_t& attr) const
    {
        if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_xmlns)
        {
            if (attr.value != "http://schemas.openxmlformats.org/spreadsheetml/2006/main")
                throw xml_structure_error("invalid namespace for worksheet element!");
            m_parent.set_default_ns(XMLNS_spreadsheetML);
        }
    }
private:
    xlsx_sheet_xml_context& m_parent;
};

}

xlsx_sheet_xml_context::xlsx_sheet_xml_context()
{
}

xlsx_sheet_xml_context::~xlsx_sheet_xml_context()
{
}

bool xlsx_sheet_xml_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_sheet_xml_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void xlsx_sheet_xml_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_sheet_xml_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == XMLNS_UNKNOWN_TOKEN)
    {
        switch (name)
        {
            case XML_worksheet:
                cout << "elem s: ";
                print_element(ns, name);
                for_each(attrs.begin(), attrs.end(), worksheet_attr_parser(*this));
            break;
        }
    }
//  warn_unhandled();
}

bool xlsx_sheet_xml_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_UNKNOWN_TOKEN)
    {
        switch (name)
        {
            case XML_worksheet:
                cout << "elem e: ";
                print_element(ns, name);
            break;
        }
    }
    return pop_stack(ns, name);
}

void xlsx_sheet_xml_context::characters(const pstring& str)
{
}

void xlsx_sheet_xml_context::set_default_ns(xmlns_token_t ns)
{
    m_default_ns = ns;
}

}
