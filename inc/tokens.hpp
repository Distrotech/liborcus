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

#ifndef __ORCUS_TOKENS_HPP__
#define __ORCUS_TOKENS_HPP__

#include "types.hpp"
#include "pstring.hpp"

namespace orcus {

class tokens
{
public:
    /** 
     * Initialize data used in this class.  Call this once before starting to 
     * use methods of this class.  This method is synchronized.
     */
    static void init();

    /** 
     * Check if a token returned from get_token() method is valid.
     * 
     * @return true if valid, false otherwise.
     */
    static bool is_valid_token(xml_token_t token);

    /** 
     * Get token from a specified name.
     *
     * @param name textural token name
     * 
     * @return token value representing the given textural token.
     */
    static xml_token_t get_token(const pstring& name);

    /** 
     * Get textural token name from a token value.
     *
     * @param token numeric token value
     * 
     * @return textural token name, or empty string in case the given token is 
     *         not valid.
     */
    static const char* get_token_name(xml_token_t token);

    /** 
     * Check if a namespace token returned from get_nstoken() method is valid.
     * 
     * @return true if valid, false otherwise.
     */
    static bool is_valid_nstoken(xmlns_token_t token);

    /** 
     * Get a namespace token from a specified name.
     *
     * @param name textural token name
     * 
     * @return token value representing the given textural token.
     */
    static xmlns_token_t get_nstoken(const pstring& name);

    /** 
     * Get textural token name from a namespace token value.
     *
     * @param token numeric token value
     * 
     * @return textural token name, or empty string in case the given token is 
     *         not valid.
     */
    static const char* get_nstoken_name(xmlns_token_t token);
};

}

#endif
