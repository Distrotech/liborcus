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

#include <sstream>

namespace orcus { namespace css {

const pseudo_element_t pseudo_element_after        = 0x0001;
const pseudo_element_t pseudo_element_before       = 0x0002;
const pseudo_element_t pseudo_element_first_letter = 0x0004;
const pseudo_element_t pseudo_element_first_line   = 0x0008;
const pseudo_element_t pseudo_element_selection    = 0x0010;
const pseudo_element_t pseudo_element_backdrop     = 0x0020;

namespace {

typedef mdds::sorted_string_map<pseudo_element_t> pe_map_type;

// Keys must be sorted.
pe_map_type::entry pseudo_elem_type_entries[] = {
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
    static pe_map_type elem_map(
        pseudo_elem_type_entries,
        ORCUS_N_ELEMENTS(pseudo_elem_type_entries),
        0);

    return elem_map.find(p, n);
}

const pseudo_class_t pseudo_class_active            = 0x0000000000000001;
const pseudo_class_t pseudo_class_checked           = 0x0000000000000002;
const pseudo_class_t pseudo_class_default           = 0x0000000000000004;
const pseudo_class_t pseudo_class_dir               = 0x0000000000000008;
const pseudo_class_t pseudo_class_disabled          = 0x0000000000000010;
const pseudo_class_t pseudo_class_empty             = 0x0000000000000020;
const pseudo_class_t pseudo_class_enabled           = 0x0000000000000040;
const pseudo_class_t pseudo_class_first             = 0x0000000000000080;
const pseudo_class_t pseudo_class_first_child       = 0x0000000000000100;
const pseudo_class_t pseudo_class_first_of_type     = 0x0000000000000200;
const pseudo_class_t pseudo_class_fullscreen        = 0x0000000000000400;
const pseudo_class_t pseudo_class_focus             = 0x0000000000000800;
const pseudo_class_t pseudo_class_hover             = 0x0000000000001000;
const pseudo_class_t pseudo_class_indeterminate     = 0x0000000000002000;
const pseudo_class_t pseudo_class_in_range          = 0x0000000000004000;
const pseudo_class_t pseudo_class_invalid           = 0x0000000000008000;
const pseudo_class_t pseudo_class_lang              = 0x0000000000010000;
const pseudo_class_t pseudo_class_last_child        = 0x0000000000020000;
const pseudo_class_t pseudo_class_last_of_type      = 0x0000000000040000;
const pseudo_class_t pseudo_class_left              = 0x0000000000080000;
const pseudo_class_t pseudo_class_link              = 0x0000000000100000;
const pseudo_class_t pseudo_class_not               = 0x0000000000200000;
const pseudo_class_t pseudo_class_nth_child         = 0x0000000000400000;
const pseudo_class_t pseudo_class_nth_last_child    = 0x0000000000800000;
const pseudo_class_t pseudo_class_nth_last_of_type  = 0x0000000001000000;
const pseudo_class_t pseudo_class_nth_of_type       = 0x0000000002000000;
const pseudo_class_t pseudo_class_only_child        = 0x0000000004000000;
const pseudo_class_t pseudo_class_only_of_type      = 0x0000000008000000;
const pseudo_class_t pseudo_class_optional          = 0x0000000010000000;
const pseudo_class_t pseudo_class_out_of_range      = 0x0000000020000000;
const pseudo_class_t pseudo_class_read_only         = 0x0000000040000000;
const pseudo_class_t pseudo_class_read_write        = 0x0000000080000000;
const pseudo_class_t pseudo_class_required          = 0x0000000100000000;
const pseudo_class_t pseudo_class_right             = 0x0000000200000000;
const pseudo_class_t pseudo_class_root              = 0x0000000400000000;
const pseudo_class_t pseudo_class_scope             = 0x0000000800000000;
const pseudo_class_t pseudo_class_target            = 0x0000001000000000;
const pseudo_class_t pseudo_class_valid             = 0x0000002000000000;
const pseudo_class_t pseudo_class_visited           = 0x0000004000000000;

namespace {

typedef mdds::sorted_string_map<pseudo_class_t> pc_map_type;

// Keys must be sorted.
pc_map_type::entry pseudo_class_type_entries[] = {
    { MDDS_ASCII("active"),           pseudo_class_active           },
    { MDDS_ASCII("checked"),          pseudo_class_checked          },
    { MDDS_ASCII("default"),          pseudo_class_default          },
    { MDDS_ASCII("dir"),              pseudo_class_dir              },
    { MDDS_ASCII("disabled"),         pseudo_class_disabled         },
    { MDDS_ASCII("empty"),            pseudo_class_empty            },
    { MDDS_ASCII("enabled"),          pseudo_class_enabled          },
    { MDDS_ASCII("first"),            pseudo_class_first            },
    { MDDS_ASCII("first-child"),      pseudo_class_first_child      },
    { MDDS_ASCII("first-of-type"),    pseudo_class_first_of_type    },
    { MDDS_ASCII("focus"),            pseudo_class_focus            },
    { MDDS_ASCII("fullscreen"),       pseudo_class_fullscreen       },
    { MDDS_ASCII("hover"),            pseudo_class_hover            },
    { MDDS_ASCII("in-range"),         pseudo_class_in_range         },
    { MDDS_ASCII("indeterminate"),    pseudo_class_indeterminate    },
    { MDDS_ASCII("invalid"),          pseudo_class_invalid          },
    { MDDS_ASCII("lang"),             pseudo_class_lang             },
    { MDDS_ASCII("last-child"),       pseudo_class_last_child       },
    { MDDS_ASCII("last-of-type"),     pseudo_class_last_of_type     },
    { MDDS_ASCII("left"),             pseudo_class_left             },
    { MDDS_ASCII("link"),             pseudo_class_link             },
    { MDDS_ASCII("not"),              pseudo_class_not              },
    { MDDS_ASCII("nth-child"),        pseudo_class_nth_child        },
    { MDDS_ASCII("nth-last-child"),   pseudo_class_nth_last_child   },
    { MDDS_ASCII("nth-last-of-type"), pseudo_class_nth_last_of_type },
    { MDDS_ASCII("nth-of-type"),      pseudo_class_nth_of_type      },
    { MDDS_ASCII("only-child"),       pseudo_class_only_child       },
    { MDDS_ASCII("only-of-type"),     pseudo_class_only_of_type     },
    { MDDS_ASCII("optional"),         pseudo_class_optional         },
    { MDDS_ASCII("out-of-range"),     pseudo_class_out_of_range     },
    { MDDS_ASCII("read-only"),        pseudo_class_read_only        },
    { MDDS_ASCII("read-write"),       pseudo_class_read_write       },
    { MDDS_ASCII("required"),         pseudo_class_required         },
    { MDDS_ASCII("right"),            pseudo_class_right            },
    { MDDS_ASCII("root"),             pseudo_class_root             },
    { MDDS_ASCII("scope"),            pseudo_class_scope            },
    { MDDS_ASCII("target"),           pseudo_class_target           },
    { MDDS_ASCII("valid"),            pseudo_class_valid            },
    { MDDS_ASCII("visited"),          pseudo_class_visited          },
};

}

pseudo_class_t to_pseudo_class(const char* p, size_t n)
{
    static pc_map_type class_map(
        pseudo_class_type_entries,
        ORCUS_N_ELEMENTS(pseudo_class_type_entries),
        0);

    return class_map.find(p, n);
}

std::string pseudo_class_to_string(pseudo_class_t val)
{
    std::ostringstream os;
    size_t n = ORCUS_N_ELEMENTS(pseudo_class_type_entries);
    const pc_map_type::entry* p = pseudo_class_type_entries;
    const pc_map_type::entry* p_end = p + n;
    for (; p != p_end; ++p)
    {
        if (val & p->value)
            os << ":" << p->key;
    }

    return os.str();
}

namespace {

typedef mdds::sorted_string_map<property_function_t> propfunc_map_type;

// Keys must be sorted.
propfunc_map_type::entry propfunc_type_entries[] = {
    { MDDS_ASCII("hsl"),  func_hsl  },
    { MDDS_ASCII("hsla"), func_hsla },
    { MDDS_ASCII("rgb"),  func_rgb  },
    { MDDS_ASCII("rgba"), func_rgba },
    { MDDS_ASCII("url"),  func_url  }
};

}

property_function_t to_property_function(const char* p, size_t n)
{
    static propfunc_map_type propfunc_map(
        propfunc_type_entries,
        ORCUS_N_ELEMENTS(propfunc_type_entries),
        func_unknown);

    return propfunc_map.find(p, n);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
