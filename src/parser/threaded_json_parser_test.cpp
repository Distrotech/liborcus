/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/threaded_json_parser.hpp>
#include <orcus/global.hpp>

#include <cassert>
#include <cstring>
#include <iostream>

using namespace orcus;
using namespace std;

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

    const json::parse_tokens_t& get_tokens() const
    {
        return m_tokens;
    }
};

void test_parser(const char* src, const json::parse_tokens_t& expected)
{
    cout << "source: " << src << endl;

    handler hdl;
    threaded_json_parser<handler> parser(src, std::strlen(src), hdl, 5);
    parser.parse();

    if (hdl.get_tokens() != expected)
    {
        cout << "Expected tokens:" << endl;
        cout << expected;
        cout << "Actual tokens:" << endl;
        cout << hdl.get_tokens();
        abort();
    }
}

void test_threaded_json_parser_basic()
{
    struct test_case
    {
        const char* source;
        json::parse_tokens_t expected;
    };

    test_case tcs[] =
    {
        {
            "[1,2,3]",
            {
                { json::parse_token_t::begin_parse },
                { json::parse_token_t::begin_array },
                { 1.0 },
                { 2.0 },
                { 3.0 },
                { json::parse_token_t::end_array },
                { json::parse_token_t::end_parse },
            }
        },
        {
            "{\"foo\": [true, false, null]}",
            {
                { json::parse_token_t::begin_parse },
                { json::parse_token_t::begin_object },
                { json::parse_token_t::object_key, ORCUS_ASCII("foo") },
                { json::parse_token_t::begin_array },
                { json::parse_token_t::boolean_true },
                { json::parse_token_t::boolean_false },
                { json::parse_token_t::null },
                { json::parse_token_t::end_array },
                { json::parse_token_t::end_object },
                { json::parse_token_t::end_parse },
            }
        }
    };

    for (size_t i = 0, n = ORCUS_N_ELEMENTS(tcs); i < n; ++i)
        test_parser(tcs[i].source, tcs[i].expected);
}

void test_threaded_json_parser_invalid()
{
    const char* invalids[] = {
        "[foo]",
        "[qwerty]",
        "[1,2] null",
        "{\"key\" 1: 12}",
        "[1,,2]",
        "\"key\": {\"inner\": 12}"
    };

    for (size_t i = 0; i < ORCUS_N_ELEMENTS(invalids); ++i)
    {
        const char* src = invalids[i];

        try
        {
            handler hdl;
            threaded_json_parser<handler> parser(src, std::strlen(src), hdl, 1);
            parser.parse();
            assert(false);
        }
        catch (const json::parse_error& e)
        {
            // works as expected.
            cout << "invalid source: " << src << endl;
        }
    }
}

int main()
{
    test_threaded_json_parser_basic();
    test_threaded_json_parser_invalid();
    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
