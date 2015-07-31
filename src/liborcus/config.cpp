/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/config.hpp"

namespace orcus {

config::config() : debug(false), structure_check(true) {}

json_config::json_config() :
    output_format(output_format_type::none),
    preserve_object_order(true) {}

json_config::~json_config() {}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
