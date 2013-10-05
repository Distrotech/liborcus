/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gnumeric_handler.hpp"
#include "gnumeric_context.hpp"

namespace orcus {

gnumeric_content_xml_handler::gnumeric_content_xml_handler(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_stream_handler(new gnumeric_content_xml_context(session_cxt, tokens, factory))
{
}

gnumeric_content_xml_handler::~gnumeric_content_xml_handler()
{
}

void gnumeric_content_xml_handler::start_document()
{
}

void gnumeric_content_xml_handler::end_document()
{
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
