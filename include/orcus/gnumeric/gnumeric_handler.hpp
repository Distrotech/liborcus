/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

#ifndef __ORCUS_GNUMERICHANDLER_HPP__
#define __ORCUS_GNUMERICHANDLER_HPP__

#include "orcus/xml_handler.hpp"
#include "orcus/xml_context.hpp"

#include <string>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

class tokens;

namespace spreadsheet { namespace iface { class import_factory; }}

/**
 * Handler for parsing the content.xml part.
 */
class gnumeric_content_xml_handler : public xml_stream_handler
{
public:
    gnumeric_content_xml_handler(const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~gnumeric_content_xml_handler();

    virtual void start_document();
    virtual void end_document();
    virtual void start_element(const sax_token_parser_element& elem);
    virtual void end_element(const sax_token_parser_element& elem);
    virtual void characters(const pstring& str);

private:
    xml_context_base& get_current_context();

private:
    spreadsheet::iface::import_factory* mp_factory;
    typedef ::boost::ptr_vector<xml_context_base> context_stack_type;
    context_stack_type m_context_stack;
};

}

#endif
