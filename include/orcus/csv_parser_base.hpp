/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CSV_PARSER_BASE_HPP
#define CSV_PARSER_BASE_HPP

#include "env.hpp"
#include "cell_buffer.hpp"
#include "parser_global.hpp"
#include "parser_base.hpp"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <cassert>
#include <sstream>

#define ORCUS_DEBUG_CSV 0

#if ORCUS_DEBUG_CSV
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus { namespace csv {

struct ORCUS_PSR_DLLPUBLIC parser_config
{
    std::string delimiters;
    char text_qualifier;
    bool trim_cell_value:1;

    parser_config();
};

class ORCUS_PSR_DLLPUBLIC parse_error : public std::exception
{
    std::string m_msg;
public:
    parse_error(const std::string& msg);
    virtual ~parse_error() throw();
    virtual const char* what() const throw();
};

class ORCUS_PSR_DLLPUBLIC parser_base : public ::orcus::parser_base
{
protected:
    const csv::parser_config& m_config;
    cell_buffer m_cell_buf;

protected:
    parser_base(const char* p, size_t n, const parser_config& config);

    /**
     * This is different from the global 'is_blank' in that it doesn't treat
     * linefeed and carriage return characters as non-blanks.
     */
    bool is_blank(char c) const;
    bool is_delim(char c) const;
    bool is_text_qualifier(char c) const;

    void skip_blanks();
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
