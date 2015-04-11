/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <orcus/sax_token_parser.hpp>
#include <orcus/tokens.hpp>
#include <orcus/xml_namespace.hpp>

#include "sax_token.hpp"

#include <cstring>

int main()
{
    const char* test_code = "<?xml version=\"1.0\"?><root/>";
    size_t len = strlen(test_code);

    sax_token_handler hdl;
    orcus::tokens token_map(NULL, 0);
    orcus::xmlns_repository repo;
    orcus::xmlns_context cxt = repo.create_context();
    orcus::sax_token_parser<sax_token_handler> parser(test_code, len, token_map, cxt, hdl);
    parser.parse();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
