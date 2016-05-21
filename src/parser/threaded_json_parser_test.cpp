/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/threaded_json_parser.hpp>

#include <cassert>

using namespace orcus;

class handler
{
    json::parse_tokens_t m_tokens;

public:
    void begin_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_parse);
    }

    void end_parse()
    {
        m_tokens.emplace_back(json::parse_token_t::end_parse);
    }

    void begin_array()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_array);
    }

    void end_array()
    {
        m_tokens.emplace_back(json::parse_token_t::end_array);
    }

    void begin_object()
    {
        m_tokens.emplace_back(json::parse_token_t::begin_object);
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        assert(!transient); // transient is never true with the threaded parser.
        m_tokens.emplace_back(json::parse_token_t::object_key, p, len);
    }

    void end_object()
    {
        m_tokens.emplace_back(json::parse_token_t::end_object);
    }

    void boolean_true()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_true);
    }

    void boolean_false()
    {
        m_tokens.emplace_back(json::parse_token_t::boolean_false);
    }

    void null()
    {
        m_tokens.emplace_back(json::parse_token_t::null);
    }

    void string(const char* p, size_t len, bool transient)
    {
        assert(!transient); // transient is never true with the threaded parser.
        m_tokens.emplace_back(json::parse_token_t::string, p, len);
    }

    void number(double val)
    {
        m_tokens.emplace_back(val);
    }
};

void test_threaded_json_parser_1()
{
    handler hdl;
    threaded_json_parser<handler> parser(nullptr, 0, hdl);
    parser.parse();
}

int main()
{
    test_threaded_json_parser_1();
    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
