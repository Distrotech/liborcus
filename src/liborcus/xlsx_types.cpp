/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_types.hpp"

#include "orcus/global.hpp"

#include <mdds/sorted_string_map.hpp>

namespace orcus {

namespace {

const char* str_unknown = "unknown";

typedef mdds::sorted_string_map<xlsx_cell_t> map_type;

// Keys must be sorted.
map_type::entry cell_type_entries[] = {
    { ORCUS_ASCII("b"), xlsx_ct_boolean },
    { ORCUS_ASCII("e"), xlsx_ct_error },
    { ORCUS_ASCII("inlineStr"), xlsx_ct_inline_string },
    { ORCUS_ASCII("n"), xlsx_ct_numeric },
    { ORCUS_ASCII("s"), xlsx_ct_shared_string },
    { ORCUS_ASCII("str"), xlsx_ct_formula_string }
};

}

xlsx_cell_t to_xlsx_cell_type(const pstring& s)
{
    static map_type ct_map(
        cell_type_entries,
        sizeof(cell_type_entries)/sizeof(cell_type_entries[0]),
        xlsx_ct_unknown);

    return ct_map.find(s.get(), s.size());
}

pstring to_string(xlsx_cell_t type)
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
    return str_unknown;
}

namespace {

typedef mdds::sorted_string_map<xlsx_rev_row_column_action_t> rca_map_type;

// Keys must be sorted.
rca_map_type::entry rca_entries[] = {
    { ORCUS_ASCII("deleteCol"), xlsx_rev_rca_delete_column },
    { ORCUS_ASCII("deleteRow"), xlsx_rev_rca_delete_row    },
    { ORCUS_ASCII("insertCol"), xlsx_rev_rca_insert_column },
    { ORCUS_ASCII("insertRow"), xlsx_rev_rca_insert_row    }
};

}

xlsx_rev_row_column_action_t to_xlsx_rev_row_column_action_type(const pstring& s)
{
    static rca_map_type rca_map(
        rca_entries,
        sizeof(rca_entries)/sizeof(rca_entries[0]),
        xlsx_rev_rca_unknown);

    return rca_map.find(s.get(), s.size());
}

pstring to_string(xlsx_rev_row_column_action_t type)
{
    switch (type)
    {
        case xlsx_rev_rca_delete_column:
            return rca_entries[0].key;
        case xlsx_rev_rca_delete_row:
            return rca_entries[1].key;
        case xlsx_rev_rca_insert_column:
            return rca_entries[2].key;
        case xlsx_rev_rca_insert_row:
            return rca_entries[3].key;
        case xlsx_rev_rca_unknown:
        default:
            ;
    }

    return str_unknown;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
