/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include "dom_tree_sax_handler.hpp"
#include "orcus/sax_parser.hpp"
#include "orcus/xml_namespace.hpp"

namespace orcus {

dom_tree_sax_handler::dom_tree_sax_handler(xmlns_context& cxt) : m_tree(cxt), m_ns_cxt(cxt) {}

void dom_tree_sax_handler::declaration()
{
    m_tree.end_declaration();
}

void dom_tree_sax_handler::start_element(const sax_parser_element& elem)
{
    xmlns_id_t ns = m_ns_cxt.get(elem.ns);
    m_tree.start_element(ns, elem.name);
}

void dom_tree_sax_handler::end_element(const sax_parser_element& elem)
{
    xmlns_id_t ns = m_ns_cxt.get(elem.ns);
    m_tree.end_element(ns, elem.name);
}

void dom_tree_sax_handler::characters(const pstring& val)
{
    m_tree.set_characters(val);
}

void dom_tree_sax_handler::attribute(const pstring& ns, const pstring& name, const pstring& val)
{
    xmlns_id_t ns_id = m_ns_cxt.get(ns);
    m_tree.set_attribute(ns_id, name, val);
}

void dom_tree_sax_handler::dump_compact(std::ostream& os)
{
    m_tree.dump_compact(os);
}

}
