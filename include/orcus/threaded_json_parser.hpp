/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_THREADED_JSON_PARSER_HPP
#define INCLUDED_ORCUS_THREADED_JSON_PARSER_HPP

#include "orcus/json_parser_thread.hpp"

#include <thread>

namespace orcus {

template<typename _Handler>
class threaded_json_parser
{
public:
    typedef _Handler handler_type;

    /**
     * Constructor.
     *
     * @param p pointer to a string stream containing JSON string.
     * @param n size of the stream.
     * @param hdl handler class instance.
     */
    threaded_json_parser(const char* p, size_t n, handler_type& hdl);

    /**
     * Call this method to start parsing.
     */
    void parse();

private:
    void thread_parse();

    void process_tokens(json::parse_tokens_t& tokens);

private:
    json::parser_thread m_parser_thread;
    handler_type& m_handler;
};

template<typename _Handler>
threaded_json_parser<_Handler>::threaded_json_parser(
    const char* p, size_t n, handler_type& hdl) :
    m_parser_thread(p, n), m_handler(hdl) {}

template<typename _Handler>
void threaded_json_parser<_Handler>::parse()
{
    std::thread t(&threaded_json_parser::thread_parse, this);

    json::parse_tokens_t tokens;

    while (m_parser_thread.next_tokens(tokens))
        process_tokens(tokens);

    if (!tokens.empty())
        process_tokens(tokens);

    t.join();
}

template<typename _Handler>
void threaded_json_parser<_Handler>::thread_parse()
{
    // Start parsing.
    m_parser_thread.start();
}

template<typename _Handler>
void threaded_json_parser<_Handler>::process_tokens(json::parse_tokens_t& tokens)
{
    // TODO : implement this.
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
