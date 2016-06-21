/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_simple_stream_handler.hpp"
#include "xml_context_base.hpp"

#include <cassert>

namespace orcus {

xml_simple_stream_handler::xml_simple_stream_handler(xml_context_base* context) :
    xml_stream_handler(context)
{
    assert(context);
}

xml_simple_stream_handler::~xml_simple_stream_handler()
{
}

xml_context_base& xml_simple_stream_handler::get_context()
{
    return get_current_context();
}

void xml_simple_stream_handler::start_document()
{
}

void xml_simple_stream_handler::end_document()
{
}

void xml_simple_stream_handler::start_element(const xml_token_element_t& elem)
{
    get_current_context().start_element(elem.ns, elem.name, elem.attrs);
}

void xml_simple_stream_handler::end_element(const xml_token_element_t& elem)
{
    get_current_context().end_element(elem.ns, elem.name);
}

void xml_simple_stream_handler::characters(const pstring& str, bool transient)
{
    get_current_context().characters(str, transient);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
