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

#ifndef __ORCUS_BASE64_HPP__
#define __ORCUS_BASE64_HPP__

#include "env.hpp"
#include <vector>
#include <string>

namespace orcus {

/**
 * Decode a based64-encoded character sequence into a sequence of bytes.
 *
 * @param p_base64 pointer to the first character of encoded character
 *                 sequence.
 * @param len_base64 length of encoded character sequence.
 * @param decoded decoded byte sequence will be put into this parameter.
 */
ORCUS_DLLPUBLIC void decode_from_base64(const char* p_base64, size_t len_base64, std::vector<char>& decoded);

/**
 * Encode a sequence of bytes into base64-encoded characters.
 *
 * @param input sequence of bytes to encode.
 * @param encoded base64-encoded character sequence representing the input
 *                bytes.
 */
ORCUS_DLLPUBLIC void encode_to_base64(const std::vector<char>& input, std::string& encoded);

}

#endif
