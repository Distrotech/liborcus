/*************************************************************************
 *
 * Copyright (c) 2011-2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

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
