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

#ifndef __ODSHANDLER_HPP__
#define __ODSHANDLER_HPP__

#include "xmlhandler.hpp"

namespace orcus {

class ods_context_base;

class ods_content_xml_handler : public xml_stream_handler
{
public:
    ods_content_xml_handler(ods_context_base* context);
    virtual ~ods_content_xml_handler();

    virtual void start_document();
    virtual void end_document();
    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_type& attrs);
    virtual void end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const char* ch, size_t len);

private:
    void table_start(const xml_attrs_type& attrs);
    void table_end();

private:
    ods_context_base* mp_context;

    typedef ::std::pair<xmlns_token_t, xml_token_t> token_pair_type;
    typedef ::std::vector<token_pair_type> stack_type;
    stack_type m_stack;
};

}

#endif
