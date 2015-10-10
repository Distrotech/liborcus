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
#include <cstddef>

namespace orcus {

class ORCUS_PSR_DLLPUBLIC parse_error : public general_error
{
    std::ptrdiff_t m_offset;  /// offset in the stream where the error occurred.
protected:
    parse_error(const std::string& msg, std::ptrdiff_t offset);
    parse_error(const std::string& cls, const std::string& msg, std::ptrdiff_t offset);

    static std::string build_message(const char* msg_before, char c, const char* msg_after);
    static std::string build_message(const char* msg_before, const char* p, size_t n, const char* msg_after);

public:
    std::ptrdiff_t offset() const;
};

class ORCUS_PSR_DLLPUBLIC parser_base
{
protected:
    const char* mp_begin;
    const char* mp_char;
    const char* mp_end;

protected:
    parser_base(const char* p, size_t n);

    bool has_char() const { return mp_char != mp_end; }
    bool has_next() const { return (mp_char+1) != mp_end; }

    void next(size_t inc=1);
    void prev(size_t dec=1);
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

    /**
     * Try to parse the next characters as double, or return NaN in case of
     * failure.
     *
     * @return double value on success, or NaN on failure.
     */
    double parse_double();

    /**
     * Determine the number of characters remaining <strong>after</strong> the
     * current character.  For instance, if the current character is on the
     * last character in the stream, this method will return 0, whereas if
     * it's on the first character, it will return the total length - 1.
     *
     * @return number of characters remaining after the current character.
     */
    size_t remaining_size() const;

    /**
     * Return the current offset from the beginning of the character stream.
     *
     * @return current offset from the beginning of the character stream.
     */
    std::ptrdiff_t offset() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
