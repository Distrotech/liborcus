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

#include "ooxml/opc_context.hpp"
#include "ooxml/opc_token_constants.hpp"
#include "global.hpp"

#include <algorithm>

using namespace std;

namespace orcus {

namespace {

class types_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    explicit types_attr_parser() : 
        m_default_ns(XMLNS_UNKNOWN_TOKEN) {}

    types_attr_parser(const types_attr_parser& r) :
        m_default_ns(r.m_default_ns) {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_xmlns)
        {
            if (attr.value != "http://schemas.openxmlformats.org/package/2006/content-types")
                throw xml_structure_error("invalid namespace for types element!");
            m_default_ns = XMLNS_ct;
        }
    }

    xmlns_token_t get_default_ns() const
    {
        return m_default_ns;
    }
private:
    xmlns_token_t m_default_ns;
};

}

opc_content_types_context::opc_content_types_context(const tokens& _tokens) :
    xml_context_base(_tokens)
{
}

bool opc_content_types_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* opc_content_types_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void opc_content_types_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child)
{
}

void opc_content_types_context::start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_attr_t> &attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_Types:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);

            print_attrs(get_tokens(), attrs);

            xmlns_token_t default_ns = 
                for_each(attrs.begin(), attrs.end(), types_attr_parser()).get_default_ns();

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_Override:
            xml_element_expected(parent, XMLNS_ct, XML_Types);
        break;
        case XML_Default:
            xml_element_expected(parent, XMLNS_ct, XML_Types);
        break;
        default:
            warn_unhandled();
    }
}

bool opc_content_types_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void opc_content_types_context::characters(const pstring &str)
{
}

}
