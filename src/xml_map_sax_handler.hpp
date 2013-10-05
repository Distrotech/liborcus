/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XML_MAP_SAX_HANDLER_HPP__
#define __ORCUS_XML_MAP_SAX_HANDLER_HPP__

#include "orcus/sax_parser.hpp"

#include <vector>

namespace orcus {

class orcus_xml;

/**
 * Sax handler for parsing XML map definition file which defines linkage
 * relationships between the XML source file and the resulting spreadsheet
 * content.
 */
class xml_map_sax_handler
{
    struct scope
    {
        pstring ns;
        pstring name;

        scope(const pstring& _ns, const pstring& _name);
    };

    std::vector<sax::parser_attribute> m_attrs;
    std::vector<scope> m_scopes;
    orcus_xml& m_app;

public:
    xml_map_sax_handler(orcus_xml& app);

    void doctype(const sax::doctype_declaration&);
    void start_declaration(const pstring& name);
    void end_declaration(const pstring& name);
    void start_element(const sax::parser_element& elem);
    void end_element(const sax::parser_element& elem);
    void characters(const pstring&, bool);
    void attribute(const sax::parser_attribute& attr);
};

void read_map_file(orcus_xml& app, const char* filepath);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
