/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_handler.hpp"
#include "xlsx_context.hpp"
#include "xlsx_sheet_context.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xlsx_sheet_xml_handler::xlsx_sheet_xml_handler(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_sheet* sheet) :
    xml_stream_handler(new xlsx_sheet_context(session_cxt, tokens, sheet))
{
}

xlsx_sheet_xml_handler::~xlsx_sheet_xml_handler()
{
}

void xlsx_sheet_xml_handler::start_document()
{
}

void xlsx_sheet_xml_handler::end_document()
{
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
