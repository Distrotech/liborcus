/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/factory.hpp"

#include "orcus/spreadsheet/shared_strings.hpp"
#include "orcus/spreadsheet/styles.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/global_settings.hpp"

namespace orcus { namespace spreadsheet {

struct import_factory_impl
{
    document& m_doc;
    row_t m_default_row_size;
    col_t m_default_col_size;

    import_global_settings m_global_settings;

    import_factory_impl(document& doc, row_t row_size, col_t col_size) :
        m_doc(doc),
        m_default_row_size(row_size),
        m_default_col_size(col_size),
        m_global_settings(doc) {}
};

import_factory::import_factory(document& doc, row_t row_size, col_t col_size) :
    mp_impl(new import_factory_impl(doc, row_size, col_size)) {}

import_factory::~import_factory()
{
    delete mp_impl;
}

iface::import_global_settings* import_factory::get_global_settings()
{
    return &mp_impl->m_global_settings;
}

iface::import_shared_strings* import_factory::get_shared_strings()
{
    return mp_impl->m_doc.get_shared_strings();
}

iface::import_styles* import_factory::get_styles()
{
    return mp_impl->m_doc.get_styles();
}

iface::import_sheet* import_factory::append_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_impl->m_doc.append_sheet(
        pstring(sheet_name, sheet_name_length), mp_impl->m_default_row_size, mp_impl->m_default_col_size);
}

iface::import_sheet* import_factory::get_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_impl->m_doc.get_sheet(pstring(sheet_name, sheet_name_length));
}

iface::import_sheet* import_factory::get_sheet(sheet_t sheet_index)
{
    return mp_impl->m_doc.get_sheet(sheet_index);
}

void import_factory::finalize()
{
    mp_impl->m_doc.finalize();
}

struct export_factory_impl
{
    document& m_doc;

    export_factory_impl(document& doc) : m_doc(doc) {}
};

export_factory::export_factory(document& doc) :
    mp_impl(new export_factory_impl(doc)) {}

export_factory::~export_factory()
{
    delete mp_impl;
}

const iface::export_sheet* export_factory::get_sheet(const char* sheet_name, size_t sheet_name_length) const
{
    return mp_impl->m_doc.get_sheet(pstring(sheet_name, sheet_name_length));
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
