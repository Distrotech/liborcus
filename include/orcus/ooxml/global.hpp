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

#ifndef __ORCUS_OOXML_GLOBAL_HPP__
#define __ORCUS_OOXML_GLOBAL_HPP__

#include "orcus/types.hpp"
#include "orcus/ooxml/ooxml_types.hpp"

#include <functional>

namespace orcus {

struct opc_rel_t;
struct xml_attr_t;

/**
 * Function object to print relationship information.
 */
struct print_opc_rel : ::std::unary_function<opc_rel_t, void>
{
    void operator() (const opc_rel_t& v) const;
};

/**
 * Function object to parse attributes of root elements.
 */
class root_element_attr_parser: ::std::unary_function<xml_attr_t, void>
{
public:
    root_element_attr_parser(
        const schema_t expected_schema, const xmlns_token_t expected_ns);

    virtual ~root_element_attr_parser();

    virtual void handle_other_attrs(const xml_attr_t& attr);

    root_element_attr_parser& operator= (const root_element_attr_parser& r);

    void operator() (const xml_attr_t& attr);

    xmlns_token_t get_default_ns() const;

private:
    xmlns_token_t m_default_ns;
    schema_t m_expected_schema;
    xmlns_token_t m_expected_ns;
};

}

#endif
