/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gnumeric_token_constants.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_cell_context.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

namespace orcus {

using namespace spreadsheet;

enum gnumeric_cell_type
{
    cell_type_bool,
    cell_type_value,
    cell_type_string,
    cell_type_formula,
    cell_type_shared_formula,
    cell_type_array,
    cell_type_unknown
};


struct gnumeric_cell_data
{
    gnumeric_cell_data() : row(0), col(0), cell_type(cell_type_unknown), shared_formula_id(-1),
                            array_rows(0), array_cols(0) {}
    row_t row;
    col_t col;
    gnumeric_cell_type cell_type;
    size_t shared_formula_id;
    row_t array_rows;
    col_t array_cols;
};

namespace {

class cell_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    cell_attr_parser()
    {
        cell_data.cell_type = cell_type_formula;
    }

    cell_attr_parser(const cell_attr_parser& r):
        cell_data(r.cell_data) {}

    void operator() (const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_Row:
                cell_data.row = atoi(attr.value.get());
                break;
            case XML_Col:
                cell_data.col = atoi(attr.value.get());
                break;
            case XML_ValueType:
            {
                int value_type = atoi(attr.value.get());
                switch (value_type)
                {
                    case 20:
                        cell_data.cell_type = cell_type_bool;
                        break;
                    case 30:
                    case 40:
                        cell_data.cell_type = cell_type_value;
                        break;
                    case 60:
                        cell_data.cell_type = cell_type_string;
                }
            }
            break;
            case XML_ExprID:
                cell_data.shared_formula_id = atoi(attr.value.get());
                cell_data.cell_type = cell_type_shared_formula;
                break;
            case XML_Rows:
                cell_data.array_rows = atoi(attr.value.get());
                cell_data.cell_type = cell_type_array;
                break;
            case XML_Cols:
                cell_data.array_cols = atoi(attr.value.get());
                cell_data.cell_type = cell_type_array;
                break;
        }
    }

    gnumeric_cell_data get_cell_data() const
    {
        return cell_data;
    }

private:
    gnumeric_cell_data cell_data;
};

}

// ============================================================================

gnumeric_cell_context::gnumeric_cell_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory, spreadsheet::iface::import_sheet* sheet) :
    xml_context_base(session_cxt, tokens),
    mp_factory(factory),
    mp_sheet(sheet)
{
}

gnumeric_cell_context::~gnumeric_cell_context()
{
}

bool gnumeric_cell_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* gnumeric_cell_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void gnumeric_cell_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void gnumeric_cell_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    push_stack(ns, name);

    if (ns == NS_gnumeric_gnm)
    {
        switch (name)
        {
            case XML_Cell:
                start_cell(attrs);
                break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool gnumeric_cell_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_gnumeric_gnm)
    {
        switch (name)
        {
            case XML_Cell:
                end_cell();
                break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void gnumeric_cell_context::characters(const pstring& str, bool /*transient*/)
{
    chars = str;
}

void gnumeric_cell_context::start_cell(const xml_attrs_t& attrs)
{
    mp_cell_data.reset(new gnumeric_cell_data);
    cell_attr_parser parser = for_each(attrs.begin(), attrs.end(), cell_attr_parser());
    *mp_cell_data = parser.get_cell_data();
}

void gnumeric_cell_context::end_cell()
{
    if (!mp_cell_data)
        return;

    col_t col = mp_cell_data->col;
    row_t row = mp_cell_data->row;
    gnumeric_cell_type cell_type = mp_cell_data->cell_type;
    switch (cell_type)
    {
        case cell_type_value:
        {
            double val = atof(chars.get());
            mp_sheet->set_value(row, col, val);
        }
        break;
        case cell_type_string:
        {
            spreadsheet::iface::import_shared_strings* shared_strings = mp_factory->get_shared_strings();
            if (!shared_strings)
                break;

            size_t id = shared_strings->add(chars.get(), chars.size());
            mp_sheet->set_string(row, col, id);
        }
        break;
        case cell_type_formula:
            mp_sheet->set_formula(row, col, spreadsheet::formula_grammar_t::gnumeric, chars.get(), chars.size());
        break;
        case cell_type_shared_formula:
        {
            if (chars.empty())
                mp_sheet->set_shared_formula(row, col, mp_cell_data->shared_formula_id);
            else
                mp_sheet->set_shared_formula(row, col, spreadsheet::formula_grammar_t::gnumeric, mp_cell_data->shared_formula_id, chars.get(), chars.size());
        }
        break;
        case cell_type_array:
        {
            mp_sheet->set_array_formula(row, col, spreadsheet::formula_grammar_t::gnumeric,
                    chars.get(), chars.size(), mp_cell_data->array_rows, mp_cell_data->array_cols);
        }
        break;
        case cell_type_bool:
        {
            bool val = chars == "TRUE";
            mp_sheet->set_bool(row, col, val);
        }
        break;
        default:
            ;
    }

    mp_cell_data.reset();
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
