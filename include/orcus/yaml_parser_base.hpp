/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_YAML_PARSER_BASE_HPP
#define INCLUDED_ORCUS_YAML_PARSER_BASE_HPP

#include "orcus/parser_base.hpp"
#include "orcus/pstring.hpp"

#include <memory>
#include <cassert>

namespace orcus { namespace yaml {

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

    // The entire line is empty.
    static const size_t parse_indent_blank_line;

    // End of stream has reached while parsing in the indent part of a line.
    static const size_t parse_indent_end_of_stream;

    parser_base() = delete;
    parser_base(const parser_base&) = delete;
    parser_base& operator=(const parser_base&) = delete;

    parser_base(const char* p, size_t n);
    ~parser_base();

    /**
     * Parse the prefix indent part of a line.
     *
     * @return number of whitespace characters encountered.
     */
    size_t parse_indent();

    /**
     * Once a non-whitespace character is reached, parse until the end of the
     * line.
     */
    pstring parse_to_end_of_line();

    /**
     * Upon encountering a '#', skip until either the line-feed or the
     * end-of-stream is reached.
     */
    void skip_comment();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
