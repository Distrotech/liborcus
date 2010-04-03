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

#ifndef __XMLHANDLER_HPP__
#define __XMLHANDLER_HPP__

#include "token_constants.hpp"

#include <cstdlib>
#include <string>
#include <vector>

namespace orcus {

typedef ::std::pair<xmlns_token_t, xml_token_t> xml_token_pair_t;
typedef ::std::vector<xml_token_pair_t>         xml_elem_stack_t;

struct xml_attr
{
    xmlns_token_t   ns;
    xml_token_t     name;
    ::std::string   value;
};

class xml_stream_handler
{
public:
    typedef ::std::vector<xml_attr> xml_attrs_type;

    xml_stream_handler();
    virtual ~xml_stream_handler() = 0;

    virtual void start_document() = 0;
    virtual void end_document() = 0;
    virtual void start_element(xmlns_token_t ns, xml_token_t name, const ::std::vector<xml_attr>& attrs) = 0;
    virtual void end_element(xmlns_token_t ns, xml_token_t name) = 0;
    virtual void characters(const char* ch, size_t len) = 0;
};

}

#endif
