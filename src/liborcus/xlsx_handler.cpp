/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_handler.hpp"
#include "xlsx_context.hpp"
#include "xlsx_sheet_context.hpp"
#include "xlsx_table_context.hpp"
#include "xlsx_pivot_context.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xlsx_sheet_xml_handler::xlsx_sheet_xml_handler(
    session_context& session_cxt, const tokens& tokens,
    spreadsheet::sheet_t sheet_id, spreadsheet::iface::import_sheet* sheet) :
    xml_stream_handler(new xlsx_sheet_context(session_cxt, tokens, sheet_id, sheet))
{
}

xlsx_sheet_xml_handler::~xlsx_sheet_xml_handler()
{
}

void xlsx_sheet_xml_handler::pop_rel_extras(opc_rel_extras_t& other)
{
    xlsx_sheet_context& cxt = static_cast<xlsx_sheet_context&>(get_root_context());
    cxt.pop_rel_extras(other);
}

xlsx_table_xml_handler::xlsx_table_xml_handler(
    session_context& session_cxt, const tokens& tokens,
    spreadsheet::iface::import_table& table) :
    xml_stream_handler(new xlsx_table_context(session_cxt, tokens, table))
{
}

xlsx_pivot_cache_xml_handler::xlsx_pivot_cache_xml_handler(
    session_context& cxt, const tokens& tokens) :
    xml_stream_handler(new xlsx_pivot_cache_context(cxt, tokens)) {}

xlsx_pivot_table_xml_handler::xlsx_pivot_table_xml_handler(
    session_context& cxt, const tokens& tokens) :
    xml_stream_handler(new xlsx_pivot_cache_context(cxt, tokens)) {}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
