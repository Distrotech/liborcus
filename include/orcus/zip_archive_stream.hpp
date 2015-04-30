/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ZIP_ARCHIVE_STREAM_HPP__
#define __ORCUS_ZIP_ARCHIVE_STREAM_HPP__

#include "env.hpp"
#include <cstdlib>
#include <cstdio>

namespace orcus {

class ORCUS_PSR_DLLPUBLIC zip_archive_stream
{
public:
    virtual ~zip_archive_stream();

    virtual size_t size() const = 0;
    virtual size_t tell() const = 0;
    virtual void seek(size_t pos) = 0;
    virtual void read(unsigned char* buffer, size_t length) const = 0;
};

/**
 * Zip archive based on file descriptor. The caller needs to provide the
 * file path to the zip archive.
 */
class ORCUS_PSR_DLLPUBLIC zip_archive_stream_fd : public zip_archive_stream
{
    FILE* m_stream;

    zip_archive_stream_fd(); // disabled

public:
    zip_archive_stream_fd(const char* filepath);
    virtual ~zip_archive_stream_fd();

    virtual size_t size() const;
    virtual size_t tell() const;
    virtual void seek(size_t pos);
    virtual void read(unsigned char* buffer, size_t length) const;
};

/**
 * Zip archive whose content is already loaded onto memory.
 */
class ORCUS_PSR_DLLPUBLIC zip_archive_stream_blob : public zip_archive_stream
{
    const unsigned char* m_blob;
    const unsigned char* m_cur;
    size_t m_size;

    zip_archive_stream_blob(); // disabled

public:
    zip_archive_stream_blob(const unsigned char* blob, size_t size);

    virtual size_t size() const;
    virtual size_t tell() const;
    virtual void seek(size_t pos);
    virtual void read(unsigned char* buffer, size_t length) const;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
