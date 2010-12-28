/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __ORCUS_OOXML_SCHEMAS_HPP__
#define __ORCUS_OOXML_SCHEMAS_HPP__

#include "ooxml_types.hpp"

namespace orcus {

extern schema_t SCH_opc_content_types;
extern schema_t SCH_opc_rels;
extern schema_t SCH_opc_rels_metadata_core_props;
extern schema_t SCH_od_rels_shared_strings;
extern schema_t SCH_od_rels_styles;
extern schema_t SCH_od_rels_theme;
extern schema_t SCH_od_rels_worksheet;
extern schema_t SCH_od_rels_extended_props;
extern schema_t SCH_od_rels_office_doc;
extern schema_t SCH_xlsx_main;

/**
 * Null-terminated array of all schema types.
 */
extern schema_t* SCH_all;

}

#endif
