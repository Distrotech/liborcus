/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CSS_PARSER_BASE_HPP
#define INCLUDED_CSS_PARSER_BASE_HPP

#include "orcus/env.hpp"
#include <string>
#include <exception>

namespace orcus { namespace css {

class ORCUS_PSR_DLLPUBLIC parse_error : public std::exception
{
    std::string m_msg;
public:
    parse_error(const std::string& msg);
    virtual ~parse_error() throw();
    virtual const char* what() const throw();
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

    void skip_blanks();
    void skip_blanks_reverse();
    void shrink_stream();
    bool skip_comment();
    void comment();
    void skip_comments_and_blanks();

protected:
    const char* mp_char;
    size_t m_pos;
    size_t m_length;
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
