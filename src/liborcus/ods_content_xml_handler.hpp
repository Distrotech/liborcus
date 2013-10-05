/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ODSHANDLER_HPP__
#define __ORCUS_ODSHANDLER_HPP__

#include "xml_stream_handler.hpp"
#include "xml_context_base.hpp"

#include <string>
#include <vector>

namespace orcus {

struct session_context;
class tokens;
class ods_content_xml_context;

namespace spreadsheet { namespace iface { class import_factory; }}

/**
 * Handler for parsing the content.xml part.
 */
class ods_content_xml_handler : public xml_stream_handler
{
public:
    ods_content_xml_handler(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~ods_content_xml_handler();

    virtual void start_document();
    virtual void end_document();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
