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

#ifndef __ORCUS_OPC_CONTEXT_HPP__
#define __ORCUS_OPC_CONTEXT_HPP__

#include "xmlcontext.hpp"

#include <unordered_set>
#include <vector>

namespace orcus {

class opc_content_types_context : public xml_context_base
{
public:
    typedef ::std::unordered_set<pstring, pstring::hash> ct_cache_type;

    opc_content_types_context(const tokens& _tokens);
    virtual ~opc_content_types_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_attr_t> &attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring &str);

private:
    ct_cache_type m_ct_cache; // content type cache;
    ::std::vector<xml_part_t> m_parts;
};

}

#endif
