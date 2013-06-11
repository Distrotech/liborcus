/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#ifndef ORCUS_SAX_PARSER_GLOBAL_HPP
#define ORCUS_SAX_PARSER_GLOBAL_HPP

#include "env.hpp"
#include "pstring.hpp"

#include <cstdlib>

namespace orcus { namespace sax {

/**
 * Document type declaration passed by sax_parser to its handler's doctype()
 * call.
 */
struct doctype_declaration
{
    enum keyword_type { keyword_public, keyword_private };

    keyword_type keyword;
    pstring root_element;
    pstring fpi;
    pstring uri;
};

/**
 * Given an encoded name (such as 'quot' and 'amp'), return a single
 * character that corresponds with the name.  The name shouldn't include the
 * leading '&' and trailing ';'.
 *
 * @param p pointer to the first character of encoded name
 * @param n length of encoded name
 *
 * @return single character that corresponds with the encoded name.  '\0' is
 *         returned if decoding fails.
 */
ORCUS_DLLPUBLIC char decode_xml_encoded_char(const char* p, size_t n);

ORCUS_DLLPUBLIC bool is_blank(char c);
ORCUS_DLLPUBLIC bool is_alpha(char c);
ORCUS_DLLPUBLIC bool is_name_char(char c);
ORCUS_DLLPUBLIC bool is_numeric(char c);

}}

#endif
