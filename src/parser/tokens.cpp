/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/tokens.hpp"
#include "orcus/pstring.hpp"

using namespace std;

namespace orcus {

tokens::tokens(const char** token_names, size_t token_name_count) :
    m_token_names(token_names), 
    m_token_name_count(token_name_count)
{
    for (size_t i = 0; i < m_token_name_count; ++i)
    {
        m_tokens.insert(
            token_map_type::value_type(
                pstring(m_token_names[i]), static_cast<xml_token_t>(i)));
    }
}

bool tokens::is_valid_token(xml_token_t token) const
{
    return token != XML_UNKNOWN_TOKEN;
}

xml_token_t tokens::get_token(const pstring& name) const
{
    token_map_type::const_iterator itr = m_tokens.find(name);
    if (itr == m_tokens.end())
        return XML_UNKNOWN_TOKEN;
    return itr->second;
}

const char* tokens::get_token_name(xml_token_t token) const
{
    if (static_cast<size_t>(token) >= m_token_name_count)
        return "";

    return m_token_names[token];
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
