/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/css_types.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <cassert>

using namespace orcus;
using namespace std;

void test_css_parse_basic1()
{
    const char* path = SRCDIR"/test/css/basic1.css";
    string strm;
    load_file_content(path, strm);
    css_document_tree doc;
    doc.load(strm);

    css_selector_t selector;
}

int main()
{
    test_css_parse_basic1();
    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
