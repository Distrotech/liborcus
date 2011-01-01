/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __ORCUS_TOKENS_BASE_HPP__
#define __ORCUS_TOKENS_BASE_HPP__

#include "orcus/types.hpp"
#include "orcus/pstring.hpp"

#include <algorithm>
#include <unordered_map>

namespace orcus {

class pstring;

class tokens
{
public:
    typedef xml_token_t     token_type;
    typedef xmlns_token_t   nstoken_type;
    typedef xml_attr_t      attr_type;
    
    static xml_token_t      XML_UNKNOWN_TOKEN;
    static xmlns_token_t    XMLNS_UNKNOWN_TOKEN;

    tokens(const char** token_names, size_t token_name_count, const char** nstoken_names, size_t nstoken_name_count);

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

    /** 
     * Check if a namespace token returned from get_nstoken() method is valid.
     * 
     * @return true if valid, false otherwise.
     */
    bool is_valid_nstoken(xmlns_token_t token) const;

    /** 
     * Get a namespace token from a specified name.
     *
     * @param name textural token name
     * 
     * @return token value representing the given textural token.
     */
    xmlns_token_t get_nstoken(const pstring& name) const;

    /** 
     * Get textural token name from a namespace token value.
     *
     * @param token numeric token value
     * 
     * @return textural token name, or empty string in case the given token is 
     *         not valid.
     */
    const char* get_nstoken_name(xmlns_token_t token) const;

private:
    typedef ::std::unordered_map<pstring, xml_token_t, pstring::hash>     token_map_type;
    typedef ::std::unordered_map<pstring, xmlns_token_t, pstring::hash>   nstoken_map_type;

    token_map_type   m_tokens;
    nstoken_map_type m_nstokens;

    const char** m_token_names;
    const char** m_nstoken_names;
    size_t m_token_name_count;
    size_t m_nstoken_name_count;
};

}

#endif
