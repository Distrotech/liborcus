/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "json_dumper.hpp"
#include "orcus/spreadsheet/document.hpp"

#include <ixion/model_context.hpp>
#include <ixion/formula_name_resolver.hpp>
#include <ixion/formula_result.hpp>
#include <mdds/multi_type_vector/collection.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

namespace orcus { namespace spreadsheet { namespace detail {

using columns_type = mdds::mtv::collection<ixion::column_store_t>;

namespace {

void dump_string(std::ofstream& file, const std::string& s)
{
    // TODO : escape this string for json.
    file << '"' << s << '"';
}

void dump_cell_value(
    std::ofstream& file, const ixion::model_context& cxt,
    const columns_type::const_iterator::value_type& node, ixion::sheet_t sheet_id)
{
    switch (node.type)
    {
        case ixion::element_type_empty:
            file << "null";
            break;
        case ixion::element_type_boolean:
        {
            auto b = node.get<ixion::boolean_element_block>();
            file << (b ? "true" : "false");
        }
        break;
        case ixion::element_type_numeric:
        {
            auto v = node.get<ixion::numeric_element_block>();
            file << v;
        }
        break;
        case ixion::element_type_string:
        {
            ixion::string_id_t sindex = node.get<ixion::string_element_block>();
            const std::string* p = cxt.get_string(sindex);
            assert(p);
            dump_string(file, *p);
        }
        break;
        case ixion::element_type_formula:
        {
            const ixion::formula_cell* cell = node.get<ixion::formula_element_block>();
            assert(cell);

            const ixion::formula_result* res = cell->get_result_cache();
            if (!res)
            {
                file << "\"#RES!\"";
                break;
            }

            switch (res->get_type())
            {
                case ixion::formula_result::rt_value:
                    file << res->get_value();
                break;
                case ixion::formula_result::rt_string:
                {
                    ixion::string_id_t sid = res->get_string();
                    const std::string* p = cxt.get_string(sid);
                    assert(p);
                    dump_string(file, *p);
                }
                break;
                case ixion::formula_result::rt_error:
                    file << "\"#ERR!\"";
                break;
            }
        }
        break;
        default:
            ;
    }
}

}

json_dumper::json_dumper(const document& doc) : m_doc(doc) {}

void json_dumper::dump(const std::string& filepath, ixion::sheet_t sheet_id) const
{
    std::ofstream file(filepath.c_str());
    if (!file)
    {
        std::cerr << "failed to create file: " << filepath << std::endl;
        return;
    }

    const ixion::model_context& cxt = m_doc.get_model_context();

    ixion::abs_range_t data_range = cxt.get_data_range(sheet_id);

    const ixion::column_stores_t* p = cxt.get_columns(sheet_id);
    if (!p)
        return;

    columns_type columns(p->begin(), p->end());

    // Only iterate through the data range.
    columns.set_collection_range(0, data_range.last.column+1);
    columns.set_element_range(0, data_range.last.row+1);

    std::vector<std::string> column_labels;
    column_labels.reserve(data_range.last.column+1);

    // Get the column labels.
    auto resolver = ixion::formula_name_resolver::get(ixion::formula_name_resolver_t::excel_a1, &cxt);
    for (ixion::col_t i = 0; i <= data_range.last.column; ++i)
        column_labels.emplace_back(resolver->get_column_name(i));

    columns_type::const_iterator it = columns.begin();

    file << "[" << std::endl;

    size_t row = it->position;
    size_t col = it->index;

    file << "    {";
    file << "\"" << column_labels[col] << "\": ";

    dump_cell_value(file, cxt, *it, sheet_id);

    size_t last_col = col;
    size_t last_row = row;

    std::for_each(++it, columns.end(),
        [&](const columns_type::const_iterator::value_type& node)
        {
            size_t row = node.position;
            size_t col = node.index;

            if (row > last_row)
                file << "}," << std::endl;

            if (col == 0)
                file << "    {";
            else
                file << ", ";

            file << "\"" << column_labels[col] << "\": ";

            dump_cell_value(file, cxt, node, sheet_id);

            last_col = node.index;
            last_row = node.position;
        }
    );

    file << "}" << std::endl << "]" << std::endl;
}

}}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
