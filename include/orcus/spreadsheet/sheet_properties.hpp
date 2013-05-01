/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef ORCUS_SHEET_PROPERTIES_HPP
#define ORCUS_SHEET_PROPERTIES_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/env.hpp"

namespace orcus { namespace spreadsheet {

class document;
class sheet;
struct sheet_properties_impl;

class sheet_properties : public iface::import_sheet_properties
{
    sheet_properties_impl* mp_impl;
public:
    sheet_properties(document& doc, sheet& sh);
    ~sheet_properties();

    virtual void set_column_width(col_t col, double width, orcus::length_unit_t unit);
    virtual void set_row_height(row_t row, double height, orcus::length_unit_t unit);
};

}}

#endif
