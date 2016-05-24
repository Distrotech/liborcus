/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_THREADED_JSON_PARSER_HPP
#define INCLUDED_ORCUS_THREADED_JSON_PARSER_HPP

#include "orcus/json_parser_thread.hpp"
#include "orcus/json_parser_base.hpp"
#include "orcus/detail/thread.hpp"

#include <algorithm>

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
    threaded_json_parser(const char* p, size_t n, handler_type& hdl, size_t min_token_size);

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
    const char* p, size_t n, handler_type& hdl, size_t min_token_size) :
    m_parser_thread(p, n, min_token_size), m_handler(hdl) {}

template<typename _Handler>
void threaded_json_parser<_Handler>::parse()
{
    std::thread t(&threaded_json_parser::thread_parse, this);
    detail::thread::scoped_guard guard(std::move(t));

    json::parse_tokens_t tokens;

    while (m_parser_thread.next_tokens(tokens))
        process_tokens(tokens);

    process_tokens(tokens);
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
    std::for_each(tokens.begin(), tokens.end(),
        [this](const json::parse_token& t)
        {
            switch (t.type)
            {
                case json::parse_token_t::begin_array:
                    m_handler.begin_array();
                    break;
                case json::parse_token_t::begin_object:
                    m_handler.begin_object();
                    break;
                case json::parse_token_t::begin_parse:
                    m_handler.begin_parse();
                    break;
                case json::parse_token_t::boolean_false:
                    m_handler.boolean_false();
                    break;
                case json::parse_token_t::boolean_true:
                    m_handler.boolean_true();
                    break;
                case json::parse_token_t::end_array:
                    m_handler.end_array();
                    break;
                case json::parse_token_t::end_object:
                    m_handler.end_object();
                    break;
                case json::parse_token_t::end_parse:
                    m_handler.end_parse();
                    break;
                case json::parse_token_t::null:
                    m_handler.null();
                    break;
                case json::parse_token_t::number:
                    m_handler.number(t.numeric_value);
                    break;
                case json::parse_token_t::object_key:
                    m_handler.object_key(t.string_value.p, t.string_value.len, false);
                    break;
                case json::parse_token_t::string:
                    m_handler.string(t.string_value.p, t.string_value.len, false);
                    break;
                case json::parse_token_t::parse_error:
                    throw json::parse_error(std::string(t.error_value.p, t.error_value.len), t.error_value.offset);
                case json::parse_token_t::unknown:
                default:
                    throw general_error("unknown token type encountered.");
            }
        }
    );

    tokens.clear();
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
