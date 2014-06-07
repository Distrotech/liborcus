/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/global_settings.hpp"
#include "orcus/spreadsheet/document.hpp"

namespace orcus { namespace spreadsheet {

struct import_global_settings_impl
{
    document& m_doc;

    formula_grammar_t m_default_grammar;

    import_global_settings_impl(document& doc) :
        m_doc(doc), m_default_grammar(formula_grammar_unknown) {}
};

import_global_settings::import_global_settings(spreadsheet::document& doc) :
    mp_impl(new import_global_settings_impl(doc)) {}

import_global_settings::~import_global_settings()
{
    delete mp_impl;
}

void import_global_settings::set_origin_date(int year, int month, int day)
{
    mp_impl->m_doc.set_origin_date(year, month, day);
}

void import_global_settings::set_default_formula_grammar(formula_grammar_t grammar)
{
    mp_impl->m_default_grammar = grammar;
}

formula_grammar_t import_global_settings::get_default_formula_grammar() const
{
    return mp_impl->m_default_grammar;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
