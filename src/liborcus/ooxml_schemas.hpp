/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_OOXML_SCHEMAS_HPP__
#define __ORCUS_OOXML_SCHEMAS_HPP__

#include "ooxml_types.hpp"

namespace orcus {

extern schema_t SCH_opc_content_types;
extern schema_t SCH_opc_rels;
extern schema_t SCH_opc_rels_metadata_core_props;
extern schema_t SCH_od_rels_connections;
extern schema_t SCH_od_rels_printer_settings;
extern schema_t SCH_od_rels_shared_strings;
extern schema_t SCH_od_rels_styles;
extern schema_t SCH_od_rels_theme;
extern schema_t SCH_od_rels_worksheet;
extern schema_t SCH_od_rels_extended_props;
extern schema_t SCH_od_rels_office_doc;
extern schema_t SCH_od_rels_table;
extern schema_t SCH_xlsx_main;

/**
 * Null-terminated array of all schema types.
 */
extern schema_t* SCH_all;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
