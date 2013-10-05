/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/base64.hpp"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace boost::archive::iterators;
using namespace std;

namespace orcus {

typedef transform_width<binary_from_base64<vector<char>::const_iterator>, 8, 6> to_binary;
typedef base64_from_binary<transform_width<vector<char>::const_iterator, 6, 8> > to_base64;

void decode_from_base64(const char* p_base64, size_t len_base64, vector<char>& decoded)
{
    if (len_base64 < 4)
        // Minimum of 4 characters required.
        return;

    vector<char> base64(p_base64, p_base64+len_base64);

    // Check the number of trailing '='s (up to 2).
    size_t pad_size = 0;
    vector<char>::reverse_iterator it = base64.rbegin();
    for (; pad_size < 2; ++pad_size, ++it)
    {
        if (*it != '=')
            break;

        *it = 'A'; // replace it with 'A' which is a base64 encoding of '\0'.
    }

    vector<char> _decoded(to_binary(base64.begin()), to_binary(base64.end()));
    _decoded.erase(_decoded.end()-pad_size, _decoded.end());

    decoded.swap(_decoded);
}

void encode_to_base64(const std::vector<char>& input, string& encoded)
{
    if (input.empty())
        return;

    string _encoded(to_base64(input.begin()), to_base64(input.end()));
    size_t pad_size = (3 - input.size() % 3) % 3;
    _encoded.append(pad_size, '=');

    encoded.swap(_encoded);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
