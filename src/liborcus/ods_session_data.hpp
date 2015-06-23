/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_ODS_SESSION_DATA_HPP
#define INCLUDED_ORCUS_ODS_SESSION_DATA_HPP

#include "session_context.hpp"

#include "orcus/spreadsheet/types.hpp"

#include <vector>
#include <memory>

namespace orcus {

struct ods_session_data : public session_context::custom_data
{
    enum formula_result_type { rt_none, rt_numeric, rt_string, rt_error };

    struct formula_result
    {
        formula_result_type type;
        double numeric_value;
        pstring string_value;

        formula_result();
    };

    struct formula
    {
        spreadsheet::sheet_t sheet;
        spreadsheet::row_t   row;
        spreadsheet::col_t   column;

        spreadsheet::formula_grammar_t grammar;
        pstring exp;


        formula_result result;

        formula(
            spreadsheet::sheet_t _sheet, spreadsheet::row_t _row, spreadsheet::col_t _col,
            spreadsheet::formula_grammar_t _grammar, const pstring& _exp);
    };

    typedef std::vector<std::unique_ptr<formula>> formulas_type;

    formulas_type m_formulas;

    virtual ~ods_session_data();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
