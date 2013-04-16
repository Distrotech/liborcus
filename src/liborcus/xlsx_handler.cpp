/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#include "xlsx_handler.hpp"
#include "xlsx_context.hpp"
#include "xlsx_sheet_context.hpp"

#include <iostream>

using namespace std;

namespace orcus {

xlsx_sheet_xml_handler::xlsx_sheet_xml_handler(
    session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_sheet* sheet) :
    xml_stream_handler(new xlsx_sheet_context(session_cxt, tokens, sheet))
{
}

xlsx_sheet_xml_handler::~xlsx_sheet_xml_handler()
{
}

void xlsx_sheet_xml_handler::start_document()
{
}

void xlsx_sheet_xml_handler::end_document()
{
}

}
