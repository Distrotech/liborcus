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

namespace {

content_type_t cts[] = {
    "application/vnd.openxmlformats-officedocument.extended-properties+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml",
    "application/vnd.openxmlformats-officedocument.theme+xml",
    "application/vnd.openxmlformats-package.core-properties+xml",
    "application/vnd.openxmlformats-package.relationships+xml",
    "application/xml",
    NULL
};

}

content_type_t CT_ooxml_extended_properties = cts[0];
content_type_t CT_ooxml_xlsx_shared_strings = cts[1];
content_type_t CT_ooxml_xlsx_sheet_main = cts[2];
content_type_t CT_ooxml_xlsx_styles = cts[3];
content_type_t CT_ooxml_xlsx_worksheet = cts[4];
content_type_t CT_ooxml_theme = cts[5];
content_type_t CT_opc_core_properties = cts[6];
content_type_t CT_opc_relationships = cts[7];
content_type_t CT_xml = cts[8];

content_type_t* CT_all = cts;

}
