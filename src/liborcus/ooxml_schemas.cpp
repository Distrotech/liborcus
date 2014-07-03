/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_schemas.hpp"

namespace orcus {

schema_t SCH_opc_content_types            = "http://schemas.openxmlformats.org/package/2006/content-types";
schema_t SCH_opc_rels                     = "http://schemas.openxmlformats.org/package/2006/relationships";
schema_t SCH_opc_rels_metadata_core_props = "http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties";
schema_t SCH_od_rels_connections          = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/connections";
schema_t SCH_od_rels_printer_settings     = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/printerSettings";
schema_t SCH_od_rels_shared_strings       = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings";
schema_t SCH_od_rels_styles               = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles";
schema_t SCH_od_rels_theme                = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/theme";
schema_t SCH_od_rels_worksheet            = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet";
schema_t SCH_od_rels_extended_props       = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties";
schema_t SCH_od_rels_office_doc           = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument";
schema_t SCH_od_rels_table                = "http://schemas.openxmlformats.org/officeDocument/2006/relationships/table";
schema_t SCH_xlsx_main                    = "http://schemas.openxmlformats.org/spreadsheetml/2006/main";


namespace {

schema_t schs[] = {
    SCH_opc_content_types,
    SCH_opc_rels,
    SCH_opc_rels_metadata_core_props,
    SCH_od_rels_connections,
    SCH_od_rels_printer_settings,
    SCH_od_rels_shared_strings,
    SCH_od_rels_styles,
    SCH_od_rels_theme,
    SCH_od_rels_worksheet,
    SCH_od_rels_extended_props,
    SCH_od_rels_office_doc,
    SCH_od_rels_table,
    SCH_xlsx_main,
    NULL
};

}

schema_t* SCH_all = schs;

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
