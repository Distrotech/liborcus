/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/sax_token_parser_thread.hpp"
#include "orcus/global.hpp"
#include "orcus/sax_token_parser.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/detail/parser_token_buffer.hpp"
#include "orcus/tokens.hpp"
#include "orcus/xml_namespace.hpp"

#include <limits>
#include <iostream>

namespace orcus { namespace sax {

parse_token::parse_token() :
    type(parse_token_t::unknown) {}

parse_token::parse_token(parse_token_t _type) :
    type(_type) {}

parse_token::parse_token(const parse_token& other) :
    type(other.type) {}

struct parser_thread::impl
{
    detail::thread::parser_token_buffer<parse_tokens_t> m_token_buffer;
    string_pool m_pool;
    parse_tokens_t m_parser_tokens; // token buffer for the parser thread.

    const char* mp_char;
    size_t m_size;
    const tokens& m_tokens;
    xmlns_context& m_ns_cxt;

    impl(const char* p, size_t n, const tokens& tks, xmlns_context& ns_cxt, size_t min_token_size, size_t max_token_size) :
        m_token_buffer(min_token_size, max_token_size),
        mp_char(p), m_size(n), m_tokens(tks), m_ns_cxt(ns_cxt)
    {
    }

    void check_and_notify()
    {
        m_token_buffer.check_and_notify(m_parser_tokens);
    }

    void notify_and_finish()
    {
        m_token_buffer.notify_and_finish(m_parser_tokens);
    }

    void start_element(const orcus::sax_token_parser_element& elem)
    {
        m_parser_tokens.emplace_back(parse_token_t::start_element);
        check_and_notify();
    }

    void end_element(const orcus::sax_token_parser_element& elem)
    {
        m_parser_tokens.emplace_back(parse_token_t::end_element);
        check_and_notify();
    }

    void characters(const orcus::pstring& val, bool transient)
    {
        m_parser_tokens.emplace_back(parse_token_t::characters);
        check_and_notify();
    }

    void start()
    {
        orcus::sax_token_parser<impl> parser(mp_char, m_size, m_tokens, m_ns_cxt, *this);
        parser.parse();
        notify_and_finish();
    }

    bool next_tokens(parse_tokens_t& tokens)
    {
        return m_token_buffer.next_tokens(tokens);
    }
};

parser_thread::parser_thread(
    const char* p, size_t n, const orcus::tokens& tks, xmlns_context& ns_cxt, size_t min_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>(
        p, n, tks, ns_cxt, min_token_size, std::numeric_limits<size_t>::max()/2)) {}

parser_thread::parser_thread(
    const char* p, size_t n, const orcus::tokens& tks, xmlns_context& ns_cxt, size_t min_token_size, size_t max_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>(
        p, n, tks, ns_cxt, min_token_size, max_token_size)) {}

parser_thread::~parser_thread()
{
}

void parser_thread::start()
{
    mp_impl->start();
}

bool parser_thread::next_tokens(parse_tokens_t& tokens)
{
    return mp_impl->next_tokens(tokens);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
