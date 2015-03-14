/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_types.hpp"

namespace orcus {

const css_pseudo_element_t css_pseudo_element_after        = 0x0001;
const css_pseudo_element_t css_pseudo_element_before       = 0x0002;
const css_pseudo_element_t css_pseudo_element_first_letter = 0x0004;
const css_pseudo_element_t css_pseudo_element_first_line   = 0x0008;
const css_pseudo_element_t css_pseudo_element_selection    = 0x0010;
const css_pseudo_element_t css_pseudo_element_backdrop     = 0x0020;

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
