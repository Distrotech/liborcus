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

class ORCUS_DLLPUBLIC import_global_settings : public spreadsheet::iface::import_global_settings
{
    spreadsheet::document& m_doc;

public:
    import_global_settings(spreadsheet::document& doc);

    virtual void set_origin_date(int year, int month, int day);
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
