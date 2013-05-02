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

#include "orcus/spreadsheet/sheet_properties.hpp"

#include <iostream>

using namespace std;

namespace orcus { namespace spreadsheet {

struct sheet_properties_impl
{
    document& m_doc;
    sheet& m_sheet;

    sheet_properties_impl(document& doc, sheet& sh) : m_doc(doc), m_sheet(sh) {}
};

sheet_properties::sheet_properties(document& doc, sheet& sh) :
    mp_impl(new sheet_properties_impl(doc, sh)) {}

sheet_properties::~sheet_properties()
{
    delete mp_impl;
}

void sheet_properties::set_column_width(col_t col, double width, orcus::length_unit_t unit)
{
    cout << "col: " << col << " width: " << width << " unit: " << unit << endl;
}

void sheet_properties::set_row_height(row_t row, double height, orcus::length_unit_t unit)
{
}

}}
