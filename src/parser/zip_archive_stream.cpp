/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "orcus/zip_archive_stream.hpp"
#include "orcus/zip_archive.hpp"

#include <sstream>

#ifdef _WIN32
#define fseeko _fseeki64
#define ftello _ftelli64
#endif

using namespace std;

namespace orcus {

zip_archive_stream::~zip_archive_stream() {}

zip_archive_stream_fd::zip_archive_stream_fd(const char* filepath) :
    m_stream(fopen(filepath, "r"))
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

}
