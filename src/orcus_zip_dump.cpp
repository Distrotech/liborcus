/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/zip_archive.hpp"
#include "orcus/zip_archive_stream.hpp"

#include <cstdlib>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    orcus::zip_archive_stream_fd stream(argv[1]);
    orcus::zip_archive archive(&stream);
    archive.load();
    size_t n = archive.get_file_entry_count();

    if (argc < 3)
    {
        for (size_t i = 0; i < n; ++i)
            archive.dump_file_entry(i);
        return EXIT_SUCCESS;
    }

    const char* entry_name = argv[2];
    archive.dump_file_entry(entry_name);

    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
