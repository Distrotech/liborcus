/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ORCUS_XML_HPP__
#define __ORCUS_ORCUS_XML_HPP__

#include "env.hpp"
#include "spreadsheet/types.hpp"

namespace orcus {

class pstring;
class xmlns_repository;
struct orcus_xml_impl;

namespace spreadsheet { namespace iface {
    class import_factory;
    class export_factory;
}}

class ORCUS_DLLPUBLIC orcus_xml
{
    orcus_xml(const orcus_xml&); // disabled
    orcus_xml& operator= (const orcus_xml&); // disabled

public:
    orcus_xml(xmlns_repository& ns_repo, spreadsheet::iface::import_factory* im_fact, spreadsheet::iface::export_factory* ex_fact);
    ~orcus_xml();

    void set_namespace_alias(const pstring& alias, const pstring& uri);

    void set_cell_link(const pstring& xpath, const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col);

    void start_range(const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col);
    void append_field_link(const pstring& xpath);
    void commit_range();

    void append_sheet(const pstring& name);

    void read_file(const char* filepath);
    void write_file(const char* filepath);

private:
    orcus_xml_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
