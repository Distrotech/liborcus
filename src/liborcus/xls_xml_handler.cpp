/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xls_xml_handler.hpp"
#include "xls_xml_context.hpp"

namespace orcus {

xls_xml_handler::xls_xml_handler(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_stream_handler(new xls_xml_context(session_cxt, tokens, factory))
{
}

xls_xml_handler::~xls_xml_handler() {}

void xls_xml_handler::start_document() {}
void xls_xml_handler::end_document() {}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
