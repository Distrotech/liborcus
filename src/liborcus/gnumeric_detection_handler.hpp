/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_GNUMERIC_DETECTION_HANDLER_HPP
#define ORCUS_GNUMERIC_DETECTION_HANDLER_HPP

#include "xml_stream_handler.hpp"

namespace orcus {

struct session_context;
class tokens;

class gnumeric_detection_handler : public xml_stream_handler
{
public:
    gnumeric_detection_handler(session_context& session_cxt, const tokens& tokens);
    virtual ~gnumeric_detection_handler();

    virtual void start_document();
    virtual void end_document();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
