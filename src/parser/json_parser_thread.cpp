/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_parser_thread.hpp"
#include "orcus/global.hpp"

namespace orcus { namespace json {

struct parser_thread::impl
{
    const char* mp_char;
    size_t m_size;

    impl(const char* p, size_t n) : mp_char(p), m_size(n) {}
};

parser_thread::parser_thread(const char* p, size_t n) :
    mp_impl(orcus::make_unique<parser_thread::impl>(p, n)) {}

parser_thread::~parser_thread() {}

void parser_thread::start()
{
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
