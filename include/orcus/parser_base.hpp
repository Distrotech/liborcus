/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PARSER_BASE_HPP
#define INCLUDED_ORCUS_PARSER_BASE_HPP

#include "orcus/env.hpp"
#include "orcus/exception.hpp"

#include <string>
#include <cstdlib>

namespace orcus {

class ORCUS_PSR_DLLPUBLIC parse_error : public general_error
{
protected:
    parse_error(const std::string& msg);

    static std::string build_message(const char* msg_before, char c, const char* msg_after);
    static std::string build_message(const char* msg_before, const char* p, size_t n, const char* msg_after);
};

class ORCUS_PSR_DLLPUBLIC parser_base
{
protected:
    const char* mp_char;
    size_t m_pos;
    size_t m_length;

protected:
    parser_base(const char* p, size_t n);

    bool has_char() const { return m_pos < m_length; }
    bool has_next() const { return m_pos + 1 < m_length; }

    void next();
    char cur_char() const;
    char next_char() const;

    void skip(const char* chars_to_skip);

    /**
     * Parse and check next characters to see if it matches specified
     * character sequence.
     *
     * @param expected null-terminated character array to match against.
     *
     * @return true if it matches specified character sequence, false
     *         otherwise.
     */
    bool parse_expected(const char* expected);

    size_t remaining_size() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
