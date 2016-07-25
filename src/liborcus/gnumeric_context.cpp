/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gnumeric_context.hpp"
#include "gnumeric_token_constants.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_sheet_context.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

}

gnumeric_content_xml_context::gnumeric_content_xml_context(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory)
{
}

gnumeric_content_xml_context::~gnumeric_content_xml_context()
{
}

bool gnumeric_content_xml_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    if (ns == NS_gnumeric_gnm && name == XML_Sheet)
        return false;

    return true;
}

xml_context_base* gnumeric_content_xml_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_gnumeric_gnm && name == XML_Sheet)
    {
        mp_child.reset(new gnumeric_sheet_context(get_session_context(), get_tokens(), mp_factory));
        mp_child->transfer_common(*this);
        return mp_child.get();
    }

    return nullptr;
}

void gnumeric_content_xml_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void gnumeric_content_xml_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& /*attrs*/)
{
    push_stack(ns, name);

    if (ns == NS_gnumeric_gnm)
    {
        switch (name)
        {
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool gnumeric_content_xml_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_gnumeric_gnm)
    {
        switch (name)
        {

        }
    }
    return pop_stack(ns, name);
}

void gnumeric_content_xml_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
