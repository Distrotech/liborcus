/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#include "xls_xml_detection_handler.hpp"
#include "xls_xml_token_constants.hpp"
#include "xls_xml_namespace_types.hpp"
#include "xml_context_base.hpp"

namespace orcus {

namespace {

/**
 * Try to parse the XML stream up to the 1st Worksheet element while
 * checking its structure along the way. If the structure up to that element
 * is correct, then we call it "detected".
 */
class xls_xml_detection_context : public xml_context_base
{
public:
    xls_xml_detection_context(session_context& session_cxt, const tokens& tokens) :
        xml_context_base(session_cxt, tokens) {}

    virtual bool can_handle_element(xmlns_id_t, xml_token_t) const
    {
        return true;
    }

    virtual xml_context_base* create_child_context(xmlns_id_t, xml_token_t)
    {
        return NULL;
    }

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
    {
        xml_token_pair_t parent = push_stack(ns, name);
        if (ns == NS_xls_xml_ss)
        {
            switch (name)
            {
                case XML_Workbook:
                    xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
                break;
                case XML_Styles:
                case XML_Worksheet:
                {
                    xml_element_expected(parent, NS_xls_xml_ss, XML_Workbook);

                    // All good. Let's call it detected.
                    throw xls_xml_detection_result(true);
                }
                break;
                case XML_Style:
                    xml_element_expected(parent, NS_xls_xml_ss, XML_Style);
                break;
                default:
                    ;
            }
        }
        else if (ns == NS_xls_xml_o)
        {
            switch (name)
            {
                case XML_DocumentProperties:
                case XML_OfficeDocumentSettings:
                    xml_element_expected(parent, NS_xls_xml_ss, XML_Workbook);
                break;
                default:
                    ;
            }
        }
        else if (ns == NS_xls_xml_x)
        {
            switch (name)
            {
                case XML_ExcelWorkbook:
                    xml_element_expected(parent, NS_xls_xml_ss, XML_Workbook);
                break;
                default:
                    ;
            }
        }
    }

    virtual bool end_element(xmlns_id_t ns, xml_token_t name)
    {
        return pop_stack(ns, name);
    }

    virtual void characters(const pstring&, bool)
    {
    }

    virtual void end_child_context(xmlns_id_t, xml_token_t, xml_context_base*)
    {
    }
};

}

xls_xml_detection_result::xls_xml_detection_result(bool result) : m_result(result) {}

bool xls_xml_detection_result::get_result() const
{
    return m_result;
}

xls_xml_detection_handler::xls_xml_detection_handler(
    session_context& session_cxt, const tokens& tokens) :
    xml_stream_handler(new xls_xml_detection_context(session_cxt, tokens))
{
}

xls_xml_detection_handler::~xls_xml_detection_handler() {}

void xls_xml_detection_handler::start_document() {}
void xls_xml_detection_handler::end_document() {}

}
