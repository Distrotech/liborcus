/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XML_SIMPLE_HANDLER_HPP__
#define __ORCUS_XML_SIMPLE_HANDLER_HPP__

#include "xml_stream_handler.hpp"

namespace orcus {

class xml_context_base;

/**
 * Simple stream handler that only uses a single context instance.
 */
class xml_simple_stream_handler : public xml_stream_handler
{
public:
    xml_simple_stream_handler(xml_context_base* context);
    ~xml_simple_stream_handler();

    xml_context_base& get_context();

    virtual void start_document();
    virtual void end_document();

    virtual void start_element(const sax_token_parser_element& elem);
    virtual void end_element(const sax_token_parser_element& elem);
    virtual void characters(const pstring& str, bool transient);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
