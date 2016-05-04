/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "string_helper.hpp"

namespace orcus {

std::vector<pstring> string_helper::split_string(const pstring& str, const char sep)
{
    std::vector<pstring> ret;

    size_t len = 0;
    const char* start = str.get();
    for (size_t i = 0, n = str.size(); i < n; ++i)
    {
        if (str[i] == sep)
        {
            ret.push_back( pstring( start, len ) );

            // if not at the end move the start string
            if (i < n-1)
                start = start + len + 1;

            len = 0;
        }
        else
            ++len;
    }
    ret.push_back( pstring( start, len ) );

    return ret;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
