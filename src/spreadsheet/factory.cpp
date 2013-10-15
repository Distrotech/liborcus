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

namespace orcus { namespace spreadsheet {

import_factory::import_factory(document* doc, row_t row_size, col_t col_size) :
    mp_document(doc), m_default_row_size(row_size), m_default_col_size(col_size) {}

import_factory::~import_factory() {}

iface::import_global_settings* import_factory::get_global_settings()
{
    return NULL;
}

iface::import_shared_strings* import_factory::get_shared_strings()
{
    return mp_document->get_shared_strings();
}

iface::import_styles* import_factory::get_styles()
{
    return mp_document->get_styles();
}

iface::import_sheet* import_factory::append_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_document->append_sheet(pstring(sheet_name, sheet_name_length), m_default_row_size, m_default_col_size);
}

iface::import_sheet* import_factory::get_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_document->get_sheet(pstring(sheet_name, sheet_name_length));
}

iface::import_sheet* import_factory::get_sheet(sheet_t sheet_index)
{
    return mp_document->get_sheet(sheet_index);
}

void import_factory::finalize()
{
    mp_document->finalize();
}

export_factory::export_factory(document* doc) : mp_document(doc) {}

export_factory::~export_factory() {}

const iface::export_sheet* export_factory::get_sheet(const char* sheet_name, size_t sheet_name_length) const
{
    return mp_document->get_sheet(pstring(sheet_name, sheet_name_length));
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
