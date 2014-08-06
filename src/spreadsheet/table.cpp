/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "table.hpp"
#include "formula_global.hpp"

#include "orcus/global.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/auto_filter.hpp"

#include <ixion/formula_name_resolver.hpp>
#include <boost/noncopyable.hpp>

namespace orcus { namespace spreadsheet {

struct table_impl : boost::noncopyable
{
    document& m_doc;
    sheet& m_sheet;

    orcus::unique_ptr<table_t> mp_data;
    table_column_t m_column;

    table_impl(document& doc, sheet& sh) : m_doc(doc), m_sheet(sh) {}
};

table::table(document& doc, sheet& sh) : mp_impl(new table_impl(doc, sh)) {}

table::~table()
{
    delete mp_impl;
}

iface::import_auto_filter* table::get_auto_filter()
{
    return NULL;
}

void table::set_range(const char* p_ref, size_t n_ref)
{
    const ixion::formula_name_resolver* resolver = mp_impl->m_doc.get_formula_name_resolver();
    if (!resolver)
        return;

    ixion::abs_range_t& range = mp_impl->mp_data->range;
    range = to_abs_range(*resolver, p_ref, n_ref);
    if (range.valid())
        range.first.sheet = range.last.sheet = mp_impl->m_sheet.get_index();
}

void table::set_identifier(size_t id)
{
    mp_impl->mp_data->identifier = id;
}

void table::set_name(const char* p, size_t n)
{
    string_pool& sp = mp_impl->m_doc.get_string_pool();
    mp_impl->mp_data->name = sp.intern(p, n).first;
}

void table::set_display_name(const char* p, size_t n)
{
    string_pool& sp = mp_impl->m_doc.get_string_pool();
    mp_impl->mp_data->display_name = sp.intern(p, n).first;
}

void table::set_totals_row_count(size_t row_count)
{
    mp_impl->mp_data->totals_row_count = row_count;
}

void table::set_column_count(size_t n)
{
    mp_impl->mp_data->columns.reserve(n);
}

void table::set_column_identifier(size_t id)
{
    mp_impl->m_column.identifier = id;
}

void table::set_column_name(const char* p, size_t n)
{
    string_pool& sp = mp_impl->m_doc.get_string_pool();
    mp_impl->m_column.name = sp.intern(p, n).first;
}

void table::set_column_totals_row_label(const char* p, size_t n)
{
    string_pool& sp = mp_impl->m_doc.get_string_pool();
    mp_impl->m_column.totals_row_label = sp.intern(p, n).first;
}

void table::set_column_totals_row_function(orcus::spreadsheet::totals_row_function_t func)
{
    mp_impl->m_column.totals_row_function = func;
}

void table::commit_column()
{
    mp_impl->mp_data->columns.push_back(mp_impl->m_column);
    mp_impl->m_column.reset();
}

void table::commit()
{
    mp_impl->m_doc.insert_table(mp_impl->mp_data.release());
    mp_impl->mp_data.reset(new table_t);
}

void table::reset()
{
    mp_impl->mp_data.reset(new table_t);
    mp_impl->m_column.reset();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
