/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/csv_parser_base.hpp"

namespace orcus { namespace csv {

parser_config::parser_config() :
    text_qualifier('\0'),
    trim_cell_value(false) {}

parse_error::parse_error(const std::string& msg) : m_msg(msg) {}

parse_error::~parse_error() throw() {}

const char* parse_error::what() const throw()
{
    return m_msg.c_str();
}

parser_base::parser_base(
    const char* p, size_t n, const csv::parser_config& config) :
    m_config(config), mp_char(p), m_pos(0), m_length(n) {}

void parser_base::next()
{
    ++m_pos;
    ++mp_char;
}

char parser_base::cur_char() const
{
    return *mp_char;
}

char parser_base::next_char() const
{
    return *(mp_char+1);
}

bool parser_base::is_blank(char c) const
{
    if (c == ' ')
        return true;

    if (c == '\t')
        // tab character.
        return true;

    return false;
}

bool parser_base::is_delim(char c) const
{
    return m_config.delimiters.find(c) != std::string::npos;
}

bool parser_base::is_text_qualifier(char c) const
{
    return m_config.text_qualifier == c;
}

void parser_base::skip_blanks()
{
    for (; has_char(); next())
    {
        if (!is_blank(*mp_char))
            break;
    }
}


}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
