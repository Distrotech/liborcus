/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_THREAD_HPP
#define INCLUDED_ORCUS_JSON_PARSER_THREAD_HPP

#include "orcus/env.hpp"

#include <memory>
#include <deque>
#include <ostream>

namespace orcus { namespace json {

enum class parse_token_t
{
    unknown,
    begin_parse,
    end_parse,
    begin_array,
    end_array,
    begin_object,
    object_key,
    end_object,
    boolean_true,
    boolean_false,
    null,
    string,
    number,
    parse_error,
};

struct ORCUS_PSR_DLLPUBLIC parse_token
{
    parse_token_t type;

    union
    {
        struct
        {
            const char* p;
            size_t len;

        } string_value;

        struct
        {
            const char* p;
            size_t len;
            std::ptrdiff_t offset;

        } error_value;

        double numeric_value;
    };

    parse_token();
    parse_token(parse_token_t _type);
    parse_token(parse_token_t _type, const char* p, size_t len);
    parse_token(parse_token_t _type, const char* p, size_t len, std::ptrdiff_t offset);
    parse_token(double value);

    parse_token(const parse_token& other);

    parse_token& operator= (parse_token) = delete;

    bool operator== (const parse_token& other) const;
    bool operator!= (const parse_token& other) const;
};

typedef std::deque<parse_token> parse_tokens_t;

ORCUS_PSR_DLLPUBLIC std::ostream& operator<< (std::ostream& os, const parse_tokens_t& tokens);

class ORCUS_PSR_DLLPUBLIC parser_thread
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    parser_thread(const char* p, size_t n, size_t max_token_size);
    ~parser_thread();

    void start();

    /**
     * Wait until new set of tokens becomes available.
     *
     * @param tokens new set of tokens.
     *
     * @return true if the parsing is still in progress (therefore more tokens
     *         to come), false if it's done i.e. this is the last token set.
     */
    bool next_tokens(parse_tokens_t& tokens);
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
