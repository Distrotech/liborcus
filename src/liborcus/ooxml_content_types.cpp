/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_content_types.hpp"

namespace orcus {

const content_type_t CT_ooxml_extended_properties   = "application/vnd.openxmlformats-officedocument.extended-properties+xml";
const content_type_t CT_ooxml_drawing               = "application/vnd.openxmlformats-officedocument.drawing+xml";
const content_type_t CT_ooxml_vml_drawing           = "application/vnd.openxmlformats-officedocument.vmlDrawing";
const content_type_t CT_ooxml_xlsx_calc_chain       = "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml";
const content_type_t CT_ooxml_xlsx_comments         = "application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml";
const content_type_t CT_ooxml_xlsx_connections      = "application/vnd.openxmlformats-officedocument.spreadsheetml.connections+xml";
const content_type_t CT_ooxml_xlsx_external_link    = "application/vnd.openxmlformats-officedocument.spreadsheetml.externalLink+xml";
const content_type_t CT_ooxml_xlsx_pivot_cache_def  = "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotCacheDefinition+xml";
const content_type_t CT_ooxml_xlsx_pivot_cache_rec  = "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotCacheRecords+xml";
const content_type_t CT_ooxml_xlsx_pivot_table      = "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotTable+xml";
const content_type_t CT_ooxml_xlsx_printer_settings = "application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings";
const content_type_t CT_ooxml_xlsx_query_table      = "application/vnd.openxmlformats-officedocument.spreadsheetml.queryTable+xml";
const content_type_t CT_ooxml_xlsx_shared_strings   = "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml";
const content_type_t CT_ooxml_xlsx_sheet_main       = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml";
const content_type_t CT_ooxml_xlsx_styles           = "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml";
const content_type_t CT_ooxml_xlsx_table            = "application/vnd.openxmlformats-officedocument.spreadsheetml.table+xml";
const content_type_t CT_ooxml_xlsx_worksheet        = "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml";
const content_type_t CT_ooxml_theme                 = "application/vnd.openxmlformats-officedocument.theme+xml";
const content_type_t CT_opc_core_properties         = "application/vnd.openxmlformats-package.core-properties+xml";
const content_type_t CT_opc_relationships           = "application/vnd.openxmlformats-package.relationships+xml";
const content_type_t CT_xml                         = "application/xml";

namespace {

content_type_t cts[] = {
    CT_ooxml_extended_properties,
    CT_ooxml_drawing,
    CT_ooxml_vml_drawing,
    CT_ooxml_xlsx_calc_chain,
    CT_ooxml_xlsx_comments,
    CT_ooxml_xlsx_connections,
    CT_ooxml_xlsx_external_link,
    CT_ooxml_xlsx_pivot_cache_def,
    CT_ooxml_xlsx_pivot_cache_rec,
    CT_ooxml_xlsx_pivot_table,
    CT_ooxml_xlsx_printer_settings,
    CT_ooxml_xlsx_query_table,
    CT_ooxml_xlsx_shared_strings,
    CT_ooxml_xlsx_sheet_main,
    CT_ooxml_xlsx_styles,
    CT_ooxml_xlsx_table,
    CT_ooxml_xlsx_worksheet,
    CT_ooxml_theme,
    CT_opc_core_properties,
    CT_opc_relationships,
    CT_xml,
    NULL
};

}

const content_type_t* CT_all = cts;

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
