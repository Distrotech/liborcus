/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_parser_base.hpp"

#include <cassert>

namespace orcus { namespace json {

parse_error::parse_error(const std::string& msg) : ::orcus::parse_error(msg) {}

void parse_error::throw_with(const char* msg_before, char c, const char* msg_after)
{
    throw parse_error(build_message(msg_before, c, msg_after));
}

void parse_error::throw_with(
    const char* msg_before, const char* p, size_t n, const char* msg_after)
{
    throw parse_error(build_message(msg_before, p, n, msg_after));
}

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n) {}

void parser_base::parse_true()
{
    static const char* expected = "true";
    if (!parse_expected(expected))
        throw parse_error("parse_true: boolean 'true' expected.");

    skip_blanks();
}

void parser_base::parse_false()
{
    static const char* expected = "false";
    if (!parse_expected(expected))
        throw parse_error("parse_false: boolean 'false' expected.");

    skip_blanks();
}

void parser_base::skip_blanks()
{
    skip(" \t\n\r");
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
