/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_GNUMERIC_CELL_CONTEXT_HPP
#define INCLUDED_ORCUS_GNUMERIC_CELL_CONTEXT_HPP

#include "xml_context_base.hpp"

#include "orcus/string_pool.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

struct gnumeric_cell_data;

class gnumeric_cell_context : public xml_context_base
{
public:
    gnumeric_cell_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory, spreadsheet::iface::import_sheet* sheet);
    virtual ~gnumeric_cell_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void start_cell(const xml_attrs_t& attrs);
    void end_cell();
private:
    spreadsheet::iface::import_factory* mp_factory;

    std::unique_ptr<gnumeric_cell_data> mp_cell_data;

    string_pool m_pool;

    /**
    * Used for temporary storage of characters
    */
    pstring chars;

    spreadsheet::iface::import_sheet* mp_sheet;
};

} // namespace orcus

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
