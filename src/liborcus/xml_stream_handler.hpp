/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XML_STREAM_HANDLER_HPP__
#define __ORCUS_XML_STREAM_HANDLER_HPP__

#include "orcus/sax_token_parser.hpp"

#include <cstdlib>
#include <string>
#include <vector>

namespace orcus {

class xml_context_base;
class xmlns_context;

class xml_stream_handler
{
    const xmlns_context* mp_ns_cxt;
    xml_context_base* mp_root_context;
    typedef std::vector<xml_context_base*> context_stack_type;
    context_stack_type m_context_stack;

    xml_stream_handler(); // disabled
public:
    xml_stream_handler(xml_context_base* root_context);
    virtual ~xml_stream_handler() = 0;

    virtual void start_document() = 0;
    virtual void end_document() = 0;

    virtual void start_element(const sax_token_parser_element& elem);
    virtual void end_element(const sax_token_parser_element& elem);
    virtual void characters(const pstring& str, bool transient);

    void set_ns_context(const xmlns_context* p);

protected:
    xml_context_base& get_current_context();
    xml_context_base& get_root_context();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
