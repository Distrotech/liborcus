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

#ifndef __ODSCONTEXT_BASE_HPP__
#define __ODSCONTEXT_BASE_HPP__

#include "xmlhandler.hpp"

namespace orcus {

typedef ::std::pair<xmlns_token_t, xml_token_t> xml_token_pair_t;
typedef ::std::vector<xml_token_pair_t>         xml_elem_stack_t;

class ods_context_base
{
public:
    virtual ~ods_context_base() = 0;
    virtual void start_context() = 0;
    virtual void end_context() = 0;

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs) = 0;
    virtual void end_element(xmlns_token_t ns, xml_token_t name) = 0;
    virtual void characters(const char* ch, size_t len) = 0;

protected:
    xml_token_pair_t push_stack(xmlns_token_t ns, xml_token_t name);
    void pop_stack(xmlns_token_t ns, xml_token_t name);
    void warn_unhandled() const;
    void warn_unexpected() const;

private:
    xml_elem_stack_t m_stack;
};

}

#endif
