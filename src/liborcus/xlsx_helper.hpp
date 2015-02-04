/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/spreadsheet/types.hpp>
#include <orcus/pstring.hpp>

namespace orcus {

bool to_rgb(const pstring& ps, spreadsheet::color_elem_t& alpha,
        spreadsheet::color_elem_t& red, spreadsheet::color_elem_t& gree,
        spreadsheet::color_elem_t& blue);

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
