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
#include "model/sheet.hpp"

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
            m_parent.set_default_ns(XMLNS_xlsx);
        }
    }
private:
    xlsx_sheet_xml_context& m_parent;
};

/**
 * Temporary sheet name to use until I parse the ooxml package structure
 * correctly in order to obtain real sheet names.  The real sheet names are
 * given in xl/workbook.xml.
 */
const char* dummy_sheet_name = "Test Sheet";

}

xlsx_sheet_xml_context::xlsx_sheet_xml_context() :
    m_default_ns(XMLNS_UNKNOWN_TOKEN),
    mp_sheet(NULL)
{
}

xlsx_sheet_xml_context::~xlsx_sheet_xml_context()
{
    delete mp_sheet;
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
    if (ns == XMLNS_UNKNOWN_TOKEN)
        ns = m_default_ns;

    xml_token_pair_t parent = push_stack(ns, name);

    switch (name)
    {
        case XML_worksheet:
            for_each(attrs.begin(), attrs.end(), worksheet_attr_parser(*this));
            print_attrs(attrs);

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = m_default_ns;

            mp_sheet = new model::sheet(pstring(dummy_sheet_name));
        break;
        case XML_sheetData:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        default:
            warn_unhandled();
    }

}

bool xlsx_sheet_xml_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_UNKNOWN_TOKEN)
        ns = m_default_ns;

    switch (name)
    {
        case XML_worksheet:
        break;
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
