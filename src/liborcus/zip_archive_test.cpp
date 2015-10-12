/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>
#include <cstdlib>

#include "orcus/zip_archive_stream.hpp"

#define ASSERT_THROW(expr) \
try \
{ \
    expr; \
    assert(0); \
} \
catch (...) \
{ \
}

using namespace std;
using namespace orcus;

void test_zip_archive_stream(zip_archive_stream* const strm, const unsigned char* const data, size_t const length)
{
    assert(strm->size() == length);
    assert(strm->tell() == 0);

    unsigned char buf[length] = {0};

    strm->read(buf, 2);
    assert(equal(data, data + 2, buf));
    assert(strm->tell() == 0);
    strm->read(buf, length);
    assert(equal(data, data + length, buf));
    ASSERT_THROW(strm->read(buf, length + 1));
    strm->read(buf, 0);

    strm->seek(2);
    assert(strm->tell() == 2);
    strm->read(buf, 2);
    assert(equal(data + 2, data + 4, buf));
    strm->seek(length);
    assert(strm->tell() == length);
    ASSERT_THROW(strm->seek(length + 1));
    assert(strm->tell() == length);
}

void test_zip_archive_stream_blob()
{
    const unsigned char data[] = "My hovercraft is full of eels.";
    zip_archive_stream_blob strm(data, sizeof(data));
    test_zip_archive_stream(&strm, data, sizeof(data));
}

int main()
{
    test_zip_archive_stream_blob();

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
