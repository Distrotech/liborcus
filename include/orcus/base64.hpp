/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
