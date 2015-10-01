/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_PARSER_GLOBAL_HPP
#define ORCUS_PARSER_GLOBAL_HPP

#include "env.hpp"

#include <sstream>

namespace orcus {

class cell_buffer;

enum class string_escape_char_t
{
    invalid,
    valid,
    control_char
};

/**
 * Stores state of string parsing.  Upon successful parsing the str points
 * to the first character of the string and the length stores the size of
 * the string.  When the parsing fails, the str value becomes nullptr and
 * the length stores the error code.
 */
struct parse_quoted_string_state
{
    static constexpr size_t error_no_closing_quote    = 1;
    static constexpr size_t error_illegal_escape_char = 2;

    const char* str;
    size_t length;

    /**
     * When true, the str pointer points to the temporary buffer storage
     * provided by the caller instead of the original character stream.  The
     * caller must allocate memory and copy the value to it before the buffer
     * content changes if the parsed string value needs to be stored.
     *
     * When false, str points to a position in the original stream, and the
     * caller doens't need to allocate memory to store the string value as
     * long as the original character stream is alive.
     */
    bool transient;
};

ORCUS_PSR_DLLPUBLIC bool is_blank(char c);
ORCUS_PSR_DLLPUBLIC bool is_alpha(char c);
ORCUS_PSR_DLLPUBLIC bool is_name_char(char c);
ORCUS_PSR_DLLPUBLIC bool is_numeric(char c);

/**
 * Check if the characater is one of allowed characters.
 *
 * @param c character to check.
 * @param allowed null-terminated character array containing all allowed
 *                characters.
 *
 * @return true if the character is one of the allowed characters, false
 *         otherwise.
 */
ORCUS_PSR_DLLPUBLIC bool is_in(char c, const char* allowed);

ORCUS_PSR_DLLPUBLIC void write_to(std::ostringstream& os, const char* p, size_t n);

ORCUS_PSR_DLLPUBLIC double parse_numeric(const char*& p, size_t max_length);

ORCUS_PSR_DLLPUBLIC long parse_integer(const char*& p, size_t max_length);

/**
 * Two single-quote characters ('') represent one single-quote character.
 */
ORCUS_PSR_DLLPUBLIC parse_quoted_string_state parse_single_quoted_string(
    const char*& p, size_t max_length, cell_buffer& buffer);

/**
 * Starting from the opening single quote position, parse string all the way
 * to the closing quote. Two single-quote characters ('') will be
 * interpreted as encoded one single-quote character.
 *
 * @param p it should point to the opening single quote character.
 * @param max_length maximum length to parse.
 *
 * @return address of the character immediately after the closing quote, or
 *         nullptr in case no closing quote is found.
 */
ORCUS_PSR_DLLPUBLIC const char* parse_to_closing_single_quote(
    const char* p, size_t max_length);

ORCUS_PSR_DLLPUBLIC parse_quoted_string_state parse_double_quoted_string(
    const char*& p, size_t max_length, cell_buffer& buffer);

/**
 * Starting from the opening double quote position, parse string all the way
 * to the closing quote. Two single-quote characters ('') will be
 * interpreted as encoded one single-quote character.
 *
 * @param p it should point to the opening single quote character.
 * @param max_length maximum length to parse.
 *
 * @return address of the character immediately after the closing quote, or
 *         nullptr in case no closing quote is found.
 */
ORCUS_PSR_DLLPUBLIC const char* parse_to_closing_double_quote(
    const char* p, size_t max_length);

/**
 * Given a character that occurs immediately after the escape character '\',
 * return what type this character is.
 *
 * @param c character that occurs immediately after the escape character
 *          '\'.
 *
 * @return enum value representing the type of escape character.
 */
ORCUS_PSR_DLLPUBLIC string_escape_char_t get_string_escape_char_type(char c);

/**
 * Clip input value to specified range in case it falls outside the range.
 *
 * @param input original input value.
 * @param low lower bound.
 * @param high upper bound.
 *
 * @return clipped value.
 */
ORCUS_PSR_DLLPUBLIC double clip(double input, double low, double high);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
