/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dom_tree_sax_handler.hpp"
#include "orcus/sax_ns_parser.hpp"

namespace orcus {

dom_tree_sax_handler::dom_tree_sax_handler(xmlns_context& cxt) : m_tree(cxt) {}

void dom_tree_sax_handler::doctype(const sax::doctype_declaration& dtd)
{
}

void dom_tree_sax_handler::start_declaration(const pstring& name)
{
    m_tree.start_declaration(name);
}

void dom_tree_sax_handler::end_declaration(const pstring& name)
{
    m_tree.end_declaration(name);
}

void dom_tree_sax_handler::start_element(const sax_ns_parser_element& elem)
{
    m_tree.start_element(elem.ns, elem.name);
}

void dom_tree_sax_handler::end_element(const sax_ns_parser_element& elem)
{
    m_tree.end_element(elem.ns, elem.name);
}

void dom_tree_sax_handler::characters(const pstring& val, bool /*transient*/)
{
    m_tree.set_characters(val);
}

void dom_tree_sax_handler::attribute(const sax_ns_parser_attribute& attr)
{
    m_tree.set_attribute(attr.ns, attr.name, attr.value);
}

void dom_tree_sax_handler::attribute(const pstring& /*name*/, const pstring& /*val*/)
{
    // We ignore XML declaration attributes for now.
}

void dom_tree_sax_handler::dump_compact(std::ostream& os)
{
    m_tree.dump_compact(os);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
