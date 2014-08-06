/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_XLSX_AUTOFILTER_CONTEXT_HPP
#define ORCUS_XLSX_AUTOFILTER_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/spreadsheet/types.hpp"

#include <vector>
#include <map>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_auto_filter;

}}

class xlsx_autofilter_context : public xml_context_base
{
public:
    typedef std::vector<pstring> match_values_type;
    typedef std::map<spreadsheet::col_t, match_values_type> column_filters_type;

    xlsx_autofilter_context(session_context& session_cxt, const tokens& tokens);
    virtual ~xlsx_autofilter_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

    void push_to_model(spreadsheet::iface::import_auto_filter& af) const;

private:
    string_pool m_pool;

    pstring m_ref_range;
    spreadsheet::col_t m_cur_col;
    match_values_type m_cur_match_values;
    column_filters_type m_column_filters;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
