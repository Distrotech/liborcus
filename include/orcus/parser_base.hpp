/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_PARSER_BASE_HPP
#define INCLUDED_ORCUS_PARSER_BASE_HPP

#include "env.hpp"

#include <cstdlib>

namespace orcus {

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
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
