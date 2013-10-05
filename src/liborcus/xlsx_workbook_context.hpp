/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XLSX_WORKBOOK_CONTEXT_HPP__
#define __ORCUS_XLSX_WORKBOOK_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include "orcus/spreadsheet/types.hpp"
#include "xlsx_types.hpp"

#include <boost/unordered_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

/**
 * Context for xl/workbook.xml.
 */
class xlsx_workbook_context : public xml_context_base
{
public:
    typedef boost::unordered_map<
        pstring, xlsx_rel_sheet_info, pstring::hash> sheet_info_type;

    xlsx_workbook_context(session_context& session_cxt, const tokens& tokens);
    virtual ~xlsx_workbook_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

    void pop_sheet_info(opc_rel_extras_t& sheets);

private:
    opc_rel_extras_t m_sheet_info;
    ::boost::ptr_vector<xlsx_rel_sheet_info> m_sheets;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
