/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SPREADSHEET_TABLE_HPP
#define ORCUS_SPREADSHEET_TABLE_HPP

#include "import_interface.hpp"

namespace orcus { namespace spreadsheet {

class table : public iface::import_table
{
public:
    virtual iface::import_auto_filter* get_auto_filter();

    virtual void set_range(const char* p_ref, size_t n_ref);
    virtual void set_identifier(size_t id);
    virtual void set_name(const char* p, size_t n);
    virtual void set_display_name(const char* p, size_t n);
    virtual void set_totals_row_count(size_t row_count);

    virtual void set_column_count(size_t n);

    virtual void set_column_identifier(size_t id);
    virtual void set_column_name(const char* p, size_t n);
    virtual void set_column_totals_row_label(const char* p, size_t n);
    virtual void set_column_totals_row_function(orcus::spreadsheet::totals_row_function_t func);
    virtual void commit_column();

    virtual void commit();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
