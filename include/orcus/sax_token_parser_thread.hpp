/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SAX_TOKEN_PARSER_THREAD_HPP
#define INCLUDED_ORCUS_SAX_TOKEN_PARSER_THREAD_HPP

#include "orcus/env.hpp"

#include <memory>
#include <vector>
#include <ostream>

namespace orcus {

class tokens;
class xmlns_context;
class pstring;
struct xml_token_element_t;

namespace sax {

enum class parse_token_t
{
    unknown,
    start_element,
    end_element,
    characters
};

struct ORCUS_PSR_DLLPUBLIC parse_token
{
    parse_token_t type;

    union
    {
        struct
        {
            const char* p;
            size_t n;

        } characters;

        const xml_token_element_t* element;
    };

    parse_token();
    parse_token(const pstring& _characters);
    parse_token(parse_token_t _type, const xml_token_element_t* _element);

    parse_token(const parse_token& other);

    parse_token& operator= (parse_token) = delete;

    bool operator== (const parse_token& other) const;
    bool operator!= (const parse_token& other) const;
};

typedef std::vector<parse_token> parse_tokens_t;

ORCUS_PSR_DLLPUBLIC std::ostream& operator<< (std::ostream& os, const parse_tokens_t& tokens);

class ORCUS_PSR_DLLPUBLIC parser_thread
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    parser_thread(const char* p, size_t n, const orcus::tokens& tks, xmlns_context& ns_cxt, size_t min_token_size);
    parser_thread(const char* p, size_t n, const orcus::tokens& tks, xmlns_context& ns_cxt, size_t min_token_size, size_t max_token_size);
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
