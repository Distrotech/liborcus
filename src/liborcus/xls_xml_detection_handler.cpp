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
#include "xml_context_base.hpp"

namespace orcus {

namespace {

class xls_xml_detection_context : public xml_context_base
{
public:
    xls_xml_detection_context(session_context& session_cxt, const tokens& tokens) :
        xml_context_base(session_cxt, tokens) {}

    virtual bool can_handle_element(xmlns_id_t, xml_token_t) const
    {
        return true;
    }

    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name)
    {
        return NULL;
    }

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
    {
    }

    virtual bool end_element(xmlns_id_t ns, xml_token_t name)
    {
        return false;
    }

    virtual void characters(const pstring& str, bool transient)
    {
    }

    virtual void end_child_context(xmlns_id_t, xml_token_t, xml_context_base*)
    {
    }
};

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
