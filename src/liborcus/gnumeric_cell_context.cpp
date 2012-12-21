/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

enum gnumeric_celltype
{
    celltype_bool,
    celltype_value,
    celltype_string,
    celltype_formula,
    celltype_shared_formula,
    celltype_unknown
};


struct gnumeric_cell_data
{
    gnumeric_cell_data() : row(0), col(0), cell_type(celltype_unknown), shared_formula_id(-1) {}
    row_t row;
    col_t col;
    gnumeric_celltype cell_type;
    size_t shared_formula_id;
};

namespace {

class cell_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    cell_attr_parser()
    {
        cell_data.cell_type = celltype_formula;
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
                        cell_data.cell_type = celltype_bool;
                        break;
                    case 30:
                    case 40:
                        cell_data.cell_type = celltype_value;
                        break;
                    case 60:
                        cell_data.cell_type = celltype_string;
                }
            }
            break;
            case XML_ExprID:
                cell_data.shared_formula_id = atoi(attr.value.get());
                cell_data.cell_type = celltype_shared_formula;
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

gnumeric_cell_context::gnumeric_cell_context(const tokens& tokens, spreadsheet::iface::import_factory* factory, spreadsheet::iface::import_sheet* sheet) :
    xml_context_base(tokens),
    mp_factory(factory),
    mp_sheet(sheet)
{
}

gnumeric_cell_context::~gnumeric_cell_context()
{
}

bool gnumeric_cell_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* gnumeric_cell_context::create_child_context(xmlns_id_t ns, xml_token_t name) const
{
    return NULL;
}

void gnumeric_cell_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
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

void gnumeric_cell_context::characters(const pstring& str)
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
    gnumeric_celltype cell_type = mp_cell_data->cell_type;
    switch (cell_type)
    {
        case celltype_value:
        {
            double val = atof(chars.get());
            mp_sheet->set_value(row, col, val);
        }
        break;
        case celltype_string:
        {
            spreadsheet::iface::import_shared_strings* shared_strings = mp_factory->get_shared_strings();
            size_t id = shared_strings->add(chars.get(), chars.size());
            mp_sheet->set_string(row, col, id);
        }
        break;
        case celltype_formula:
            mp_sheet->set_formula(row, col, spreadsheet::gnumeric, chars.get(), chars.size());
        break;
        case celltype_shared_formula:
        {
            if (chars.empty())
                mp_sheet->set_shared_formula(row, col, mp_cell_data->shared_formula_id);
            else
                mp_sheet->set_shared_formula(row, col, spreadsheet::gnumeric, mp_cell_data->shared_formula_id, chars.get(), chars.size());
        }
        break;
        default:
            ;
    }

    mp_cell_data.reset();
}

}
