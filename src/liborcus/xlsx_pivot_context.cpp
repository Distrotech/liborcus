/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_pivot_context.hpp"

namespace orcus {

xlsx_pivot_table_context::xlsx_pivot_table_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_table_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_pivot_table_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_pivot_table_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_pivot_table_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    warn_unhandled();
}

bool xlsx_pivot_table_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_table_context::characters(const pstring& str, bool transient)
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
