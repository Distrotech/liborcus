/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XLSX_TYPES_HPP__
#define __ORCUS_XLSX_TYPES_HPP__

#include "ooxml_types.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {

class import_sheet;

}}

struct xlsx_rel_sheet_info : public opc_rel_extra
{
    pstring name;
    size_t  id;

    xlsx_rel_sheet_info() : id(0) {}

    virtual ~xlsx_rel_sheet_info() {}
};

struct xlsx_rel_table_info : public opc_rel_extra
{
    spreadsheet::iface::import_sheet* sheet_interface;

    xlsx_rel_table_info() : sheet_interface(nullptr) {}

    virtual ~xlsx_rel_table_info() {}
};

enum xlsx_cell_t
{
    xlsx_ct_unknown = 0,
    xlsx_ct_boolean,
    xlsx_ct_error,
    xlsx_ct_numeric,
    xlsx_ct_inline_string,
    xlsx_ct_shared_string,
    xlsx_ct_formula_string,
};

xlsx_cell_t to_xlsx_cell_type(const pstring& s);

pstring to_string(xlsx_cell_t type);

enum xlsx_rev_row_column_action_t
{
    xlsx_rev_rca_unknown = 0,
    xlsx_rev_rca_delete_column,
    xlsx_rev_rca_delete_row,
    xlsx_rev_rca_insert_column,
    xlsx_rev_rca_insert_row
};

xlsx_rev_row_column_action_t to_xlsx_rev_row_column_action_type(const pstring& s);

pstring to_string(xlsx_rev_row_column_action_t type);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
