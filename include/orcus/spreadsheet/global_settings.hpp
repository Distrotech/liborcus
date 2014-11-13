/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_GLOBAL_SETTINGS_HPP
#define ORCUS_GLOBAL_SETTINGS_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"

namespace orcus { namespace spreadsheet {

class document;

struct import_global_settings_impl;

class ORCUS_SPM_DLLPUBLIC import_global_settings : public spreadsheet::iface::import_global_settings
{
    import_global_settings_impl* mp_impl;

public:
    import_global_settings(document& doc);
    virtual ~import_global_settings();

    virtual void set_origin_date(int year, int month, int day);

    virtual void set_default_formula_grammar(orcus::spreadsheet::formula_grammar_t grammar);

    virtual orcus::spreadsheet::formula_grammar_t get_default_formula_grammar() const;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
