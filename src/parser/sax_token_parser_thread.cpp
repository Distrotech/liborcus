/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/sax_token_parser_thread.hpp"
#include "orcus/global.hpp"

namespace orcus { namespace sax {

parse_token::parse_token() :
    type(parse_token_t::unknown) {}

parse_token::parse_token(parse_token_t _type) :
    type(_type) {}

parse_token::parse_token(const parse_token& other) :
    type(other.type) {}

struct parser_thread::impl
{
};

parser_thread::parser_thread(const char* p, size_t n, size_t min_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>())
{
}

parser_thread::parser_thread(const char* p, size_t n, size_t min_token_size, size_t max_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>())
{
}

parser_thread::~parser_thread()
{
}

void parser_thread::start()
{
}

bool parser_thread::next_tokens(parse_tokens_t& tokens)
{
    return false;
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
