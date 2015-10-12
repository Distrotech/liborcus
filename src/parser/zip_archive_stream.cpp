/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/zip_archive_stream.hpp"
#include "orcus/zip_archive.hpp"

#include <sstream>
#include <cstring>

#ifdef _WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

using namespace std;

namespace orcus {

zip_archive_stream::~zip_archive_stream() {}

zip_archive_stream_fd::zip_archive_stream_fd(const char* filepath) :
    m_stream(fopen(filepath, "rb"))
{
    if (!m_stream)
    {
        // Fail early at instantiation time.
        ostringstream os;
        os << "failed to open " << filepath << " for reading";
        throw zip_error(os.str());
    }
}

zip_archive_stream_fd::~zip_archive_stream_fd()
{
    if (m_stream)
        fclose(m_stream);
}

size_t zip_archive_stream_fd::size() const
{
    if (fseeko(m_stream, 0, SEEK_END))
        throw zip_error("failed to set seek position to the end of stream.");

    return ftello(m_stream);
}

size_t zip_archive_stream_fd::tell() const
{
    return ftello(m_stream);
}

void zip_archive_stream_fd::read(unsigned char* buffer, size_t length) const
{
    size_t size_read = fread(buffer, 1, length, m_stream);
    if (size_read != length)
        throw zip_error("actual size read doesn't match what was expected.");
}

void zip_archive_stream_fd::seek(size_t pos)
{
    if (fseeko(m_stream, pos, SEEK_SET))
    {
        ostringstream os;
        os << "failed to set seek position to " << pos << ".";
        throw zip_error(os.str());
    }
}


zip_archive_stream_blob::zip_archive_stream_blob(const unsigned char* blob, size_t size) :
    m_blob(blob), m_cur(blob), m_size(size) {}

size_t zip_archive_stream_blob::size() const
{
    return m_size;
}

size_t zip_archive_stream_blob::tell() const
{
    return std::distance(m_blob, m_cur);
}

void zip_archive_stream_blob::seek(size_t pos)
{
    if (pos > m_size)
    {
        ostringstream os;
        os << "failed to seek position to " << pos << ".";
        throw zip_error(os.str());
    }
    m_cur = m_blob + pos;
}

void zip_archive_stream_blob::read(unsigned char* buffer, size_t length) const
{
    if (!length)
        return;
    // First, make sure we have enough blob to satisfy the requested stream length.
    const size_t length_available = m_size - tell();
    if (length_available < length)
        throw zip_error("There is not enough stream left to fill requested length.");

    memcpy(buffer, m_cur, length);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
