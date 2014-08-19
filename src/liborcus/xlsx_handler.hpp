/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XLSX_HANDLER_HPP__
#define __ORCUS_XLSX_HANDLER_HPP__

#include "xml_stream_handler.hpp"
#include "xml_context_base.hpp"

#include "orcus/spreadsheet/types.hpp"

#include <string>
#include <vector>

namespace orcus {

struct session_context;
struct opc_rel_extras_t;

namespace spreadsheet { namespace iface {

class import_sheet;
class import_table;

}}

class xlsx_sheet_xml_handler : public xml_stream_handler
{
public:
    xlsx_sheet_xml_handler(
        session_context& cxt, const tokens& tokens,
        spreadsheet::sheet_t sheet_id, spreadsheet::iface::import_sheet* import_sheet);

    virtual ~xlsx_sheet_xml_handler();

    void pop_rel_extras(opc_rel_extras_t& other);
};

class xlsx_table_xml_handler : public xml_stream_handler
{
public:
    xlsx_table_xml_handler(
        session_context& cxt, const tokens& tokens, spreadsheet::iface::import_table& table);
};

class xlsx_pivot_cache_def_xml_handler : public xml_stream_handler
{
public:
    xlsx_pivot_cache_def_xml_handler(session_context& cxt, const tokens& tokens);
};

class xlsx_pivot_cache_rec_xml_handler : public xml_stream_handler
{
public:
    xlsx_pivot_cache_rec_xml_handler(session_context& cxt, const tokens& tokens);
};

class xlsx_pivot_table_xml_handler : public xml_stream_handler
{
public:
    xlsx_pivot_table_xml_handler(session_context& cxt, const tokens& tokens);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
