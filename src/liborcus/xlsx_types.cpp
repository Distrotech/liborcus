/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_types.hpp"
#include "sorted_string_map.hpp"

namespace orcus {

namespace {

typedef sorted_string_map<xlsx_cell_type> map_type;

// Keys must be sorted.
map_type::entry cell_type_entries[] = {
    { "b", xlsx_ct_boolean },
    { "e", xlsx_ct_error },
    { "inlineStr", xlsx_ct_inline_string },
    { "n", xlsx_ct_numeric },
    { "s", xlsx_ct_shared_string },
    { "str", xlsx_ct_formula_string }
};

}

xlsx_cell_type to_xlsx_cell_type(const pstring& s)
{
    static map_type ct_map(
        cell_type_entries,
        sizeof(cell_type_entries)/sizeof(cell_type_entries[0]),
        xlsx_ct_unknown);

    return ct_map.find(s.get(), s.size());
}

pstring to_string(xlsx_cell_type type)
{
    switch (type)
    {
        case xlsx_ct_boolean:
            return cell_type_entries[0].key;
        case xlsx_ct_error:
            return cell_type_entries[1].key;
        case xlsx_ct_inline_string:
            return cell_type_entries[2].key;
        case xlsx_ct_numeric:
            return cell_type_entries[3].key;
        case xlsx_ct_shared_string:
            return cell_type_entries[4].key;
        case xlsx_ct_formula_string:
            return cell_type_entries[5].key;
        default:
            ;
    }
    return "unknown";
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
