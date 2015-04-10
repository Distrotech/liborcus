/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_TOKENS_HPP
#define INCLUDED_ORCUS_TOKENS_HPP

#include "orcus/types.hpp"
#include "orcus/pstring.hpp"

#include <algorithm>
#include <boost/unordered_map.hpp>

namespace orcus {

class pstring;

class ORCUS_PSR_DLLPUBLIC tokens
{
public:

    tokens(const char** token_names, size_t token_name_count);

    /**
     * Check if a token returned from get_token() method is valid.
     *
     * @return true if valid, false otherwise.
     */
    bool is_valid_token(xml_token_t token) const;

    /**
     * Get token from a specified name.
     *
     * @param name textural token name
     *
     * @return token value representing the given textural token.
     */
    xml_token_t get_token(const pstring& name) const;

    /**
     * Get textural token name from a token value.
     *
     * @param token numeric token value
     *
     * @return textural token name, or empty string in case the given token is
     *         not valid.
     */
    const char* get_token_name(xml_token_t token) const;

private:
    typedef boost::unordered_map<pstring, xml_token_t, pstring::hash>     token_map_type;
    token_map_type   m_tokens;
    const char** m_token_names;
    size_t m_token_name_count;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
