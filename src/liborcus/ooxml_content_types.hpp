/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_OOXML_CONTENT_TYPES_HPP__
#define __ORCUS_OOXML_CONTENT_TYPES_HPP__

#include "ooxml_types.hpp"

namespace orcus {

extern const content_type_t CT_ooxml_extended_properties;
extern const content_type_t CT_ooxml_drawing;
extern const content_type_t CT_ooxml_vml_drawing;
extern const content_type_t CT_ooxml_xlsx_calc_chain;
extern const content_type_t CT_ooxml_xlsx_comments;
extern const content_type_t CT_ooxml_xlsx_connections;
extern const content_type_t CT_ooxml_xlsx_external_link;
extern const content_type_t CT_ooxml_xlsx_pivot_cache_def;
extern const content_type_t CT_ooxml_xlsx_pivot_cache_rec;
extern const content_type_t CT_ooxml_xlsx_pivot_table;
extern const content_type_t CT_ooxml_xlsx_printer_settings;
extern const content_type_t CT_ooxml_xlsx_query_table;
extern const content_type_t CT_ooxml_xlsx_shared_strings;
extern const content_type_t CT_ooxml_xlsx_sheet_main;
extern const content_type_t CT_ooxml_xlsx_styles;
extern const content_type_t CT_ooxml_xlsx_table;
extern const content_type_t CT_ooxml_xlsx_worksheet;
extern const content_type_t CT_ooxml_xlsx_usernames;
extern const content_type_t CT_ooxml_xlsx_rev_headers;
extern const content_type_t CT_ooxml_xlsx_rev_log;
extern const content_type_t CT_ooxml_theme;
extern const content_type_t CT_opc_core_properties;
extern const content_type_t CT_opc_relationships;
extern const content_type_t CT_xml;

/**
 * Null-terminated array of all content types.
 */
extern const content_type_t* CT_all;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
