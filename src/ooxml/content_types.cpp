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

#include "ooxml/content_types.hpp"

namespace orcus {

content_type_t CT_ooxml_extended_properties   = "application/vnd.openxmlformats-officedocument.extended-properties+xml";
content_type_t CT_ooxml_drawing               = "application/vnd.openxmlformats-officedocument.drawing+xml";
content_type_t CT_ooxml_vml_drawing           = "application/vnd.openxmlformats-officedocument.vmlDrawing";
content_type_t CT_ooxml_xlsx_calc_chain       = "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml";
content_type_t CT_ooxml_xlsx_comments         = "application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml";
content_type_t CT_ooxml_xlsx_external_link    = "application/vnd.openxmlformats-officedocument.spreadsheetml.externalLink+xml";
content_type_t CT_ooxml_xlsx_pivot_table      = "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotTable+xml";
content_type_t CT_ooxml_xlsx_printer_settings = "application/vnd.openxmlformats-officedocument.spreadsheetml.printerSettings";
content_type_t CT_ooxml_xlsx_shared_strings   = "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml";
content_type_t CT_ooxml_xlsx_sheet_main       = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml";
content_type_t CT_ooxml_xlsx_styles           = "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml";
content_type_t CT_ooxml_xlsx_worksheet        = "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml";
content_type_t CT_ooxml_theme                 = "application/vnd.openxmlformats-officedocument.theme+xml";
content_type_t CT_opc_core_properties         = "application/vnd.openxmlformats-package.core-properties+xml";
content_type_t CT_opc_relationships           = "application/vnd.openxmlformats-package.relationships+xml";
content_type_t CT_xml                         = "application/xml";

namespace {

content_type_t cts[] = {
    CT_ooxml_extended_properties,
    CT_ooxml_drawing,
    CT_ooxml_vml_drawing,
    CT_ooxml_xlsx_calc_chain,
    CT_ooxml_xlsx_comments,
    CT_ooxml_xlsx_external_link,
    CT_ooxml_xlsx_pivot_table,
    CT_ooxml_xlsx_printer_settings,
    CT_ooxml_xlsx_shared_strings,
    CT_ooxml_xlsx_sheet_main,
    CT_ooxml_xlsx_styles,
    CT_ooxml_xlsx_worksheet,
    CT_ooxml_theme,
    CT_opc_core_properties,
    CT_opc_relationships,
    CT_xml,
    NULL
};

}

content_type_t* CT_all = cts;

}
