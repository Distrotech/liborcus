/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "yaml.hpp"
#include <orcus/yaml_parser.hpp>

#include <cstring>

const char* test_code =
"section-one:"
"  - item 1"
"  - item 2"
"";

int main()
{
    size_t n = strlen(test_code);

    yaml_parser_handler hdl;
    orcus::yaml_parser<yaml_parser_handler> parser(test_code, n, hdl);
    parser.parse();
    return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
