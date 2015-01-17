/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/spreadsheet/export_interface.hpp"

namespace orcus { namespace spreadsheet { namespace iface {

import_shared_strings::~import_shared_strings() {}

import_styles::~import_styles() {}

import_sheet_properties::~import_sheet_properties() {}

import_data_table::~import_data_table() {}

import_auto_filter::~import_auto_filter() {}

import_table::~import_table() {}

import_conditional_format::~import_conditional_format() {}

import_auto_filter* import_table::get_auto_filter()
{
    return NULL;
}

import_sheet::~import_sheet() {}

import_sheet_properties* import_sheet::get_sheet_properties()
{
    return NULL;
}

import_data_table* import_sheet::get_data_table()
{
    return NULL;
}

import_auto_filter* import_sheet::get_auto_filter()
{
    return NULL;
}

import_table* import_sheet::get_table()
{
    return NULL;
}

import_conditional_format* import_sheet::get_conditional_format()
{
    return NULL;
}

import_global_settings::~import_global_settings() {}

import_factory::~import_factory() {}

import_global_settings* import_factory::get_global_settings()
{
    return NULL;
}

import_shared_strings* import_factory::get_shared_strings()
{
    return NULL;
}

import_styles* import_factory::get_styles()
{
    return NULL;
}

export_sheet::~export_sheet() {}

export_factory::~export_factory() {}

}}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
