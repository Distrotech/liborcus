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
    struct attr
    {
        pstring ns;
        pstring name;
        pstring val;

        attr(const pstring& _ns, const pstring& _name, const pstring& _val);
    };

    struct scope
    {
        pstring ns;
        pstring name;

        scope(const pstring& _ns, const pstring& _name);
    };

    std::vector<attr> m_attrs;
    std::vector<scope> m_scopes;
    orcus_xml& m_app;

public:
    xml_map_sax_handler(orcus_xml& app);

    void doctype(const sax::doctype_declaration&);
    void start_declaration(const pstring& name);
    void end_declaration(const pstring& name);
    void start_element(const sax_parser_element& elem);
    void end_element(const sax_parser_element& elem);
    void characters(const pstring&);
    void attribute(const pstring& ns, const pstring& name, const pstring& val);
};

void read_map_file(orcus_xml& app, const char* filepath);

}

#endif
