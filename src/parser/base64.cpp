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

#include "orcus/base64.hpp"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace boost::archive::iterators;
using namespace std;

namespace orcus {

typedef transform_width<binary_from_base64<vector<char>::const_iterator>, 8, 6> to_binary;
typedef base64_from_binary<transform_width<vector<char>::const_iterator,6,8> > to_base64;

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
