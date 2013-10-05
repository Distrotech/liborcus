/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ORCUS_XLSX_HPP
#define ORCUS_ORCUS_XLSX_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct xlsx_rel_sheet_info;
struct orcus_xlsx_impl;
class xlsx_opc_handler;

class ORCUS_DLLPUBLIC orcus_xlsx : public iface::import_filter
{
    friend class xlsx_opc_handler;

    orcus_xlsx(const orcus_xlsx&); // disabled
    orcus_xlsx& operator= (const orcus_xlsx&); // disabled

public:
    orcus_xlsx(spreadsheet::iface::import_factory* factory);
    ~orcus_xlsx();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:

    void read_workbook(const std::string& dir_path, const std::string& file_name);

    /**
     * Parse a sheet xml part that contains data stored in a single sheet.
     */
    void read_sheet(const std::string& dir_path, const std::string& file_name, const xlsx_rel_sheet_info* data);

    /**
     * Parse sharedStrings.xml part that contains a list of strings referenced
     * in the document.
     */
    void read_shared_strings(const std::string& dir_path, const std::string& file_name);

    void read_styles(const std::string& dir_path, const std::string& file_name);

private:
    orcus_xlsx_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
