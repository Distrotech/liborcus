/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#include "factory.hpp"

#include "shared_strings.hpp"
#include "styles.hpp"
#include "sheet.hpp"
#include "document.hpp"

namespace orcus { namespace spreadsheet {

factory::factory(document* doc) :
    mp_document(doc)
{
}

factory::~factory()
{
}

iface::shared_strings* factory::get_shared_strings()
{
    return mp_document->get_shared_strings();
}

iface::styles* factory::get_styles()
{
    return mp_document->get_styles();
}

iface::sheet* factory::append_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_document->append_sheet(pstring(sheet_name, sheet_name_length));
}

iface::sheet* factory::get_sheet(const char* sheet_name, size_t sheet_name_length)
{
    return mp_document->get_sheet(pstring(sheet_name, sheet_name_length));
}

}}
