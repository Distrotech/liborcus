/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XLSX_SHEET_CONTEXT_HPP__
#define __ORCUS_XLSX_SHEET_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include "orcus/spreadsheet/types.hpp"
#include "orcus/string_pool.hpp"

namespace orcus {

struct session_context;

namespace spreadsheet { namespace iface {
    class import_sheet;
}}

/**
 * Top-level context for xl/worksheets/sheet<num>.xml.
 */
class xlsx_sheet_context : public xml_context_base
{
public:
    enum cell_type {
        cell_type_string,
        cell_type_formula_string,
        cell_type_value,
        cell_type_boolean,
        cell_type_error,
        cell_type_inline_string
    };

    xlsx_sheet_context(
        session_context& session_cxt, const tokens& tokens,
        spreadsheet::sheet_t sheet_id, spreadsheet::iface::import_sheet* import_sheet);
    virtual ~xlsx_sheet_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void end_element_cell();

private:
    spreadsheet::iface::import_sheet* mp_sheet; /// sheet model instance for the loaded document.
    string_pool m_pool;
    spreadsheet::sheet_t m_sheet_id; /// ID of this sheet.
    spreadsheet::row_t m_cur_row;
    spreadsheet::col_t m_cur_col;
    cell_type    m_cur_cell_type;
    size_t       m_cur_cell_xf;
    pstring      m_cur_str;
    pstring      m_cur_value;
    pstring      m_cur_formula_type;
    pstring      m_cur_formula_ref;
    pstring      m_cur_formula_str;
    int          m_cur_shared_formula_id; /// 0-based shared formula index.
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
