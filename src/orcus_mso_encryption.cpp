/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/mso/encryption_info.hpp"
#include "orcus/stream.hpp"

using namespace orcus;
using namespace std;

int main(int argc, char** argv) try
{
    if (argc != 2)
        return EXIT_FAILURE;

    mso::encryption_info_reader reader;
    string strm = load_file_content(argv[1]);

    if (strm.empty())
        return EXIT_FAILURE;

    reader.read(&strm[0], strm.size());

    return EXIT_SUCCESS;
}
catch (...)
{
    return EXIT_FAILURE;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
