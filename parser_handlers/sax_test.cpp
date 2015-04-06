/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/sax_parser.hpp>
#include "sax.hpp"

#include <cstring>

int main()
{
    const char* test_code = "<?xml version=\"1.0\"?><root/>";
    size_t len = strlen(test_code);

    sax_parser_handler hdl;
    orcus::sax_parser<sax_parser_handler> parser(test_code, len, hdl);
    parser.parse();
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
