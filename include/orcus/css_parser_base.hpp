/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CSS_PARSER_BASE_HPP
#define INCLUDED_CSS_PARSER_BASE_HPP

#include "orcus/env.hpp"
#include "orcus/css_types.hpp"
#include "orcus/exception.hpp"

#include <string>
#include <exception>

namespace orcus { namespace css {

class ORCUS_PSR_DLLPUBLIC parse_error : public general_error
{
public:
    parse_error(const std::string& msg);

    static void throw_with(const char* msg_before, char c, const char* msg_after);
    static void throw_with(const char* msg_before, const char* p, size_t n, const char* msg_after);
};

class ORCUS_PSR_DLLPUBLIC parser_base
{
public:
    parser_base(const char* p, size_t n);

protected:
    void next();
    char cur_char() const;

    /**
     * The caller must ensure that the next character exists.
     */
    char next_char() const;

    size_t remaining_size() const;
    bool has_char() const;

    void identifier(const char*& p, size_t& len, const char* extra = NULL);
    uint8_t parse_uint8();
    double parse_double();

    void literal(const char*& p, size_t& len, char quote);
    void skip_to(const char*& p, size_t& len, char c);
    void skip_blanks();
    void skip_blanks_reverse();
    void shrink_stream();
    bool skip_comment();
    void comment();
    void skip_comments_and_blanks();
    void set_combinator(char c, css::combinator_t combinator);
    void reset_before_block();

protected:
    const char* mp_char;
    size_t m_pos;
    size_t m_length;

    size_t m_simple_selector_count;
    combinator_t m_combinator;
};


}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
