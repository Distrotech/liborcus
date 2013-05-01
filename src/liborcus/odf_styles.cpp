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

#include "odf_styles.hpp"

namespace orcus {

odf_style::odf_style() : family(style_family_unknown), column_data(NULL) {}
odf_style::odf_style(const pstring& _name, odf_style_family _family) :
    name(_name), family(_family), column_data(NULL)
{
    switch (family)
    {
        case style_family_table_column:
            column_data = new column;
        break;
        case style_family_table_row:
            row_data = new row;
        break;
        case style_family_table:
            table_data = new table;
        break;
        case style_family_graphic:
            graphic_data = new graphic;
        break;
        case style_family_paragraph:
            paragraph_data = new paragraph;
        break;
        case style_family_text:
            text_data = new text;
        break;
        case style_family_unknown:
        default:
            ;
    }
}

odf_style::~odf_style()
{
    switch (family)
    {
        case style_family_table_column:
            delete column_data;
        break;
        case style_family_table_row:
            delete row_data;
        break;
        case style_family_table:
            delete table_data;
        break;
        case style_family_graphic:
            delete graphic_data;
        break;
        case style_family_paragraph:
            delete paragraph_data;
        break;
        case style_family_text:
            delete text_data;
        break;
        case style_family_unknown:
        default:
            ;
    }
}

}
