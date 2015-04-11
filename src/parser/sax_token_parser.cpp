/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/sax_token_parser.hpp"
#include "orcus/tokens.hpp"

namespace orcus {

sax_token_handler_base::sax_token_handler_base(const tokens& _tokens) :
    m_tokens(_tokens) {}

xml_token_t sax_token_handler_base::tokenize(const pstring& name) const
{
    xml_token_t token = XML_UNKNOWN_TOKEN;
    if (!name.empty())
        token = m_tokens.get_token(name);
    return token;
}

void sax_token_handler_base::set_element(const sax_ns_parser_element& elem)
{
    m_elem.ns = elem.ns;
    m_elem.name = tokenize(elem.name);
    m_elem.raw_name = elem.name;
}

void sax_token_handler_base::attribute(const pstring& /*name*/, const pstring& /*val*/)
{
    // Right now we don't process XML declaration.
}

void sax_token_handler_base::attribute(const sax_ns_parser_attribute& attr)
{
    m_elem.attrs.push_back(
       xml_token_attr_t(
           attr.ns, tokenize(attr.name), attr.name,
           attr.value, attr.transient));
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
