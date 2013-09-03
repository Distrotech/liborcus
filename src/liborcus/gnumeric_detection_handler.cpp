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

#include "gnumeric_detection_handler.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_token_constants.hpp"
#include "xml_context_base.hpp"
#include "detection_result.hpp"

namespace orcus {

namespace {

class gnumeric_detection_context : public xml_context_base
{
public:
    gnumeric_detection_context(session_context& session_cxt, const tokens& tokens) :
        xml_context_base(session_cxt, tokens) {}

    virtual bool can_handle_element(xmlns_id_t, xml_token_t) const { return true; };
    virtual xml_context_base* create_child_context(xmlns_id_t, xml_token_t) { return NULL; }
    virtual void characters(const pstring&, bool) {}
    virtual void end_child_context(xmlns_id_t, xml_token_t, xml_context_base*) {}

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& /*attrs*/)
    {
        xml_token_pair_t parent = push_stack(ns, name);
        if (ns == NS_gnumeric_gnm)
        {
            switch (name)
            {
                case XML_Workbook:
                    xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
                break;
                case XML_Version:
                case XML_Attributes:
                case XML_Calculation:
                case XML_SheetNameIndex:
                case XML_Geometry:
                case XML_Sheets:
                    xml_element_expected(parent, NS_gnumeric_gnm, XML_Workbook);
                break;
                case XML_Attribute:
                    xml_element_expected(parent, NS_gnumeric_gnm, XML_Attributes);
                break;
                case XML_SheetName:
                    xml_element_expected(parent, NS_gnumeric_gnm, XML_SheetNameIndex);
                break;
                case XML_Sheet:
                    xml_element_expected(parent, NS_gnumeric_gnm, XML_Sheets);
                    throw detection_result(true);
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
};

}

gnumeric_detection_handler::gnumeric_detection_handler(session_context& session_cxt, const tokens& tokens) :
    xml_stream_handler(new gnumeric_detection_context(session_cxt, tokens)) {}

gnumeric_detection_handler::~gnumeric_detection_handler() {}

void gnumeric_detection_handler::start_document() {}
void gnumeric_detection_handler::end_document() {}

}
