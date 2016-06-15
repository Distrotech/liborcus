/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_styles.hpp"

namespace orcus {

odf_style::odf_style() : family(style_family_unknown), column_data(NULL) {}
odf_style::odf_style(const pstring& _name, odf_style_family _family,
        const pstring& parent) :
    name(_name), family(_family),
    parent_name(parent), column_data(NULL)
{
    switch (family)
    {
        case style_family_table_column:
            column_data = new column;
        break;
        case style_family_table_row:
            row_data = new row;
        break;
        case style_family_table_cell:
            cell_data = new cell;
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
        case style_family_table_cell:
            delete cell_data;
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

number_formatting_style::number_formatting_style(const pstring& style_name, const bool volatile_style)
{
    name = style_name;
    is_volatile = volatile_style;
}


}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
