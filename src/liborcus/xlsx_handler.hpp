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

#ifndef __ORCUS_XLSX_HANDLER_HPP__
#define __ORCUS_XLSX_HANDLER_HPP__

#include "xml_stream_handler.hpp"
#include "xml_context_base.hpp"

#include <string>
#include <vector>

namespace orcus {

struct session_context;

namespace spreadsheet { namespace iface {

class import_sheet;

}}

class xlsx_sheet_xml_handler : public xml_stream_handler
{
public:
    xlsx_sheet_xml_handler(session_context& cxt, const tokens& tokens, spreadsheet::iface::import_sheet* import_sheet);
    virtual ~xlsx_sheet_xml_handler();

    virtual void start_document();
    virtual void end_document();
};

}

#endif
