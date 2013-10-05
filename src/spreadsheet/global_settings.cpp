/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/global_settings.hpp"
#include "orcus/spreadsheet/document.hpp"

namespace orcus { namespace spreadsheet {

import_global_settings::import_global_settings(spreadsheet::document& doc) :
    m_doc(doc) {}

void import_global_settings::set_origin_date(int year, int month, int day)
{
    m_doc.set_origin_date(year, month, day);
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
