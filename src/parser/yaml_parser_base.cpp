/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_parser_base.hpp"
#include "orcus/global.hpp"

#include <limits>

namespace orcus { namespace yaml {

struct parser_base::impl
{
};

const size_t parser_base::parse_indent_blank_line    = std::numeric_limits<size_t>::max();
const size_t parser_base::parse_indent_end_of_stream = std::numeric_limits<size_t>::max() - 1;

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(make_unique<impl>()) {}

parser_base::~parser_base() {}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
