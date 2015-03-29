/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CSS_TYPES_HPP
#define INCLUDED_ORCUS_CSS_TYPES_HPP

#include "orcus/env.hpp"
#include <cstdlib>
#include <inttypes.h>
#include <string>

namespace orcus { namespace css {

enum combinator_t
{
    combinator_descendant,   /// 'E F' where F is a descendant of E.
    combinator_direct_child, /// 'E > F' where F is a direct child of E.
    combinator_next_sibling  /// 'E + F' where F is a direct sibling of E where E precedes F.
};

/**
 * List of functions used as property values.
 */
enum property_function_t
{
    func_unknown = 0,
    func_hsl,
    func_hsla,
    func_rgb,
    func_rgba,
    func_url
};

enum property_value_t
{
    property_value_none = 0,
    property_value_string,
    property_value_hsl,
    property_value_hsla,
    property_value_rgb,
    property_value_rgba,
    property_value_url
};

typedef uint16_t pseudo_element_t;
typedef uint64_t pseudo_class_t;

ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_after;
ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_before;
ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_first_letter;
ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_first_line;
ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_selection;
ORCUS_PSR_DLLPUBLIC extern const pseudo_element_t pseudo_element_backdrop;

ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_active;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_checked;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_default;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_dir;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_disabled;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_empty;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_enabled;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_first;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_first_child;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_first_of_type;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_fullscreen;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_focus;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_hover;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_indeterminate;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_in_range;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_invalid;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_lang;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_last_child;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_last_of_type;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_left;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_link;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_not;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_nth_child;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_nth_last_child;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_nth_last_of_type;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_nth_of_type;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_only_child;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_only_of_type;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_optional;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_out_of_range;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_read_only;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_read_write;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_required;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_right;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_root;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_scope;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_target;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_valid;
ORCUS_PSR_DLLPUBLIC extern const pseudo_class_t pseudo_class_visited;

/**
 * Convert a textural representation of a pseudo element into its numerical
 * representation.
 */
ORCUS_PSR_DLLPUBLIC pseudo_element_t to_pseudo_element(const char* p, size_t n);

/**
 * Convert a textural representation of a pseudo class into its numerical
 * representation.
 */
ORCUS_PSR_DLLPUBLIC pseudo_class_t to_pseudo_class(const char* p, size_t n);

ORCUS_PSR_DLLPUBLIC std::string pseudo_class_to_string(pseudo_class_t val);

ORCUS_PSR_DLLPUBLIC property_function_t to_property_function(const char* p, size_t n);

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
