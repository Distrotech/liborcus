/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_BASE_HPP
#define INCLUDED_ORCUS_JSON_PARSER_BASE_HPP

#include "orcus/parser_base.hpp"
#include "orcus/parser_global.hpp"

#include <memory>

namespace orcus { namespace json {

enum class escape_char_t
{
    illegal,
    legal,
    control_char
};

/**
 * Given a character that occurs immediately after the escape character '\',
 * return what type this character is.
 *
 * @param c character that occurs immediately after the escape character
 *          '\'.
 *
 * @return enum value representing the type of escape character.
 */
ORCUS_PSR_DLLPUBLIC escape_char_t get_escape_char_type(char c);

class ORCUS_PSR_DLLPUBLIC parse_error : public ::orcus::parse_error
{
public:
    parse_error(const std::string& msg);

    static void throw_with(const char* msg_before, char c, const char* msg_after);
    static void throw_with(const char* msg_before, const char* p, size_t n, const char* msg_after);
};

class ORCUS_PSR_DLLPUBLIC parser_base : public ::orcus::parser_base
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

protected:

    parser_base() = delete;
    parser_base(const parser_base&) = delete;
    parser_base& operator=(const parser_base&) = delete;

    parser_base(const char* p, size_t n);
    ~parser_base();

    void parse_true();
    void parse_false();
    void parse_null();
    long parse_long_or_throw();
    double parse_double_or_throw();

    parse_quoted_string_state parse_string();

    void skip_blanks();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
