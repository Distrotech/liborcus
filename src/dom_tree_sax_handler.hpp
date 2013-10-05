/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_DOM_TREE_SAX_HANDLER_HPP__
#define __ORCUS_DOM_TREE_SAX_HANDLER_HPP__

#include "orcus/dom_tree.hpp"

#include <ostream>

namespace orcus {

namespace sax {
    struct doctype_declaration;
}

class xmlns_context;
struct sax_ns_parser_element;
struct sax_ns_parser_attribute;
class pstring;

/**
 * Sax handler just to wrap a dom tree instance.
 */
class dom_tree_sax_handler
{
    dom_tree m_tree;
public:
    dom_tree_sax_handler(xmlns_context& cxt);

    void doctype(const sax::doctype_declaration& dtd);
    void start_declaration(const pstring& name);
    void end_declaration(const pstring& name);
    void start_element(const sax_ns_parser_element& elem);
    void end_element(const sax_ns_parser_element& elem);
    void characters(const pstring& val, bool transient);
    void attribute(const sax_ns_parser_attribute& attr);
    void attribute(const pstring& name, const pstring& val);
    void dump_compact(std::ostream& os);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
