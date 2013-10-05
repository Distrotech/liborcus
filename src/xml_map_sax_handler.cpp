/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_map_sax_handler.hpp"

#include "orcus/spreadsheet/types.hpp"
#include "orcus/orcus_xml.hpp"
#include "orcus/global.hpp"
#include "orcus/sax_parser.hpp"
#include "orcus/stream.hpp"

using namespace std;

namespace orcus {

xml_map_sax_handler::scope::scope(const pstring& _ns, const pstring& _name) :
    ns(_ns), name(_name) {}

xml_map_sax_handler::xml_map_sax_handler(orcus_xml& app) : m_app(app) {}

void xml_map_sax_handler::doctype(const sax::doctype_declaration& dtd)
{
}

void xml_map_sax_handler::start_declaration(const pstring& name)
{
}

void xml_map_sax_handler::end_declaration(const pstring& name)
{
    m_attrs.clear();
}

void xml_map_sax_handler::start_element(const sax::parser_element& elem)
{
    pstring xpath, sheet;
    spreadsheet::row_t row = -1;
    spreadsheet::col_t col = -1;
    vector<sax::parser_attribute>::const_iterator it = m_attrs.begin(), it_end = m_attrs.end();

    if (elem.name == "ns")
    {
        // empty alias is associated with default namespace.
        pstring alias, uri;
        for (; it != it_end; ++it)
        {
            if (it->name == "alias")
                alias = it->value;
            else if (it->name == "uri")
                uri = it->value;
        }

        if (!uri.empty())
            m_app.set_namespace_alias(alias, uri);
    }
    else if (elem.name == "cell")
    {
        for (; it != it_end; ++it)
        {
            if (it->name == "xpath")
                xpath = it->value;
            else if (it->name == "sheet")
                sheet = it->value;
            else if (it->name == "row")
                row = strtol(it->value.get(), NULL, 10);
            else if (it->name == "column")
                col = strtol(it->value.get(), NULL, 10);
        }

        m_app.set_cell_link(xpath, sheet, row, col);
    }
    else if (elem.name == "range")
    {
        for (; it != it_end; ++it)
        {
            if (it->name == "sheet")
                sheet = it->value;
            else if (it->name == "row")
                row = strtol(it->value.get(), NULL, 10);
            else if (it->name == "column")
                col = strtol(it->value.get(), NULL, 10);
        }

        m_app.start_range(sheet, row, col);
    }
    else if (elem.name == "field")
    {
        for (; it != it_end; ++it)
        {
            if (it->name == "xpath")
            {
                xpath = it->value;
                break;
            }
        }

        m_app.append_field_link(xpath);
    }
    else if (elem.name == "sheet")
    {
        pstring sheet_name;
        for (; it != it_end; ++it)
        {
            if (it->name == "name")
            {
                sheet_name = it->value;
                break;
            }
        }

        if (!sheet_name.empty())
            m_app.append_sheet(sheet_name);
    }

    m_scopes.push_back(scope(elem.ns, elem.name));
    m_attrs.clear();
}

void xml_map_sax_handler::end_element(const sax::parser_element& elem)
{
    if (elem.name == "range")
        m_app.commit_range();

    m_scopes.pop_back();
}

void xml_map_sax_handler::characters(const pstring&, bool) {}

void xml_map_sax_handler::attribute(const sax::parser_attribute& attr)
{
    m_attrs.push_back(attr);
}

void read_map_file(orcus_xml& app, const char* filepath)
{
    string strm;
    load_file_content(filepath, strm);
    if (strm.empty())
        return;

    xml_map_sax_handler handler(app);
    sax_parser<xml_map_sax_handler> parser(strm.c_str(), strm.size(), handler);
    parser.parse();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
