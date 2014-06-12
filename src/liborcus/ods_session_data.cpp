/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ods_session_data.hpp"

namespace orcus {

ods_session_data::formula_result::formula_result() : type(rt_none) {}

ods_session_data::formula::formula(
    spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _col,
    spreadsheet::formula_grammar_t _grammar, const pstring& _exp) :
    sheet(_sheet), row(_row), column(_col), grammar(_grammar), exp(_exp) {}

ods_session_data::~ods_session_data() {}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
