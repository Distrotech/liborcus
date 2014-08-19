/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ORCUS_IMPORT_XLSX_HPP
#define ORCUS_ORCUS_IMPORT_XLSX_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_table;
}}

class ORCUS_DLLPUBLIC import_xlsx
{
private:
    import_xlsx(); // deleted
    import_xlsx(const import_xlsx&); // deleted
    import_xlsx& operator=(const import_xlsx&); // deleted

public:
    static void read_table(const char* p, size_t n, spreadsheet::iface::import_table* data);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
