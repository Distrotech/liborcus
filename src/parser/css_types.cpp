/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_types.hpp"
#include "orcus/global.hpp"
#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>

namespace orcus { namespace css {

const pseudo_element_t pseudo_element_after        = 0x0001;
const pseudo_element_t pseudo_element_before       = 0x0002;
const pseudo_element_t pseudo_element_first_letter = 0x0004;
const pseudo_element_t pseudo_element_first_line   = 0x0008;
const pseudo_element_t pseudo_element_selection    = 0x0010;
const pseudo_element_t pseudo_element_backdrop     = 0x0020;

namespace {

typedef mdds::sorted_string_map<pseudo_element_t> map_type;

// Keys must be sorted.
map_type::entry pseudo_elem_type_entries[] = {
    { MDDS_ASCII("after"),        pseudo_element_after        },
    { MDDS_ASCII("backdrop"),     pseudo_element_backdrop     },
    { MDDS_ASCII("before"),       pseudo_element_before       },
    { MDDS_ASCII("first-letter"), pseudo_element_first_letter },
    { MDDS_ASCII("first-line"),   pseudo_element_first_line   },
    { MDDS_ASCII("selection"),    pseudo_element_selection    },
};

}

pseudo_element_t to_pseudo_element(const char* p, size_t n)
{
    static map_type elem_map(
        pseudo_elem_type_entries,
        ORCUS_N_ELEMENTS(pseudo_elem_type_entries),
        0);

    return elem_map.find(p, n);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
