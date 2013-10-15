/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_session_data.hpp"

namespace orcus {

xlsx_session_data::formula::formula(
    spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _column,
    const std::string& _exp) :
    sheet(_sheet), row(_row), column(_column), exp(_exp), array(false) {}

xlsx_session_data::formula::formula(
    spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _column,
    const std::string& _exp, const std::string& _range) :
    sheet(_sheet), row(_row), column(_column), exp(_exp), range(_range), array(true) {}

xlsx_session_data::shared_formula::shared_formula(
    spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _column, size_t _identifier) :
    sheet(_sheet), row(_row), column(_column), identifier(_identifier), master(false) {}

xlsx_session_data::shared_formula::shared_formula(
    spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _column,
    size_t _identifier, const std::string& _formula, const std::string& _range) :
    sheet(_sheet), row(_row), column(_column),
    identifier(_identifier), formula(_formula), range(_range), master(true) {}

xlsx_session_data::~xlsx_session_data()
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
