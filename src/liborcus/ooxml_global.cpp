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

#include "orcus/ooxml/global.hpp"
#include "orcus/ooxml/ooxml_types.hpp"
#include "orcus/ooxml/ooxml_token_constants.hpp"

#include <iostream>

using namespace std;

namespace orcus {

void print_opc_rel::operator() (const opc_rel_t& v) const
{
    using namespace std;
    cout << v.rid << ": " << v.target << " (" << v.type << ")" << endl;
}

// ============================================================================

root_element_attr_parser::root_element_attr_parser(
    const schema_t expected_schema, const xmlns_token_t expected_ns) :
    m_default_ns(XMLNS_UNKNOWN_TOKEN),
    m_expected_schema(expected_schema),
    m_expected_ns(expected_ns) {}

root_element_attr_parser::~root_element_attr_parser() {}

void root_element_attr_parser::handle_other_attrs(const xml_token_attr_t&) {}

root_element_attr_parser& root_element_attr_parser::operator= (const root_element_attr_parser& r)
{
    m_default_ns = r.m_default_ns;
    m_expected_schema = r.m_expected_schema;
    m_expected_ns = r.m_expected_ns;
    return *this;
}

void root_element_attr_parser::operator() (const xml_token_attr_t& attr)
{
    if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_xmlns)
    {
        if (attr.value == m_expected_schema)
            m_default_ns = m_expected_ns;
    }
    else
        handle_other_attrs(attr);
}

xmlns_token_t root_element_attr_parser::get_default_ns() const
{
    return m_default_ns;
}

}
