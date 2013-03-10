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

#ifndef __ORCUS_ZIP_ARCHIVE_STREAM_HPP__
#define __ORCUS_ZIP_ARCHIVE_STREAM_HPP__

#include "env.hpp"
#include <cstdlib>
#include <cstdio>

namespace orcus {

class ORCUS_DLLPUBLIC zip_archive_stream
{
public:
    virtual ~zip_archive_stream();

    virtual size_t tell() const = 0;
    virtual void seek(size_t pos) = 0;
    virtual void seek_end() = 0;
    virtual void read(char* buffer, size_t length) const = 0;
};

class ORCUS_DLLPUBLIC zip_archive_stream_fd : public zip_archive_stream
{
    FILE* m_stream;

    zip_archive_stream_fd(); // disabled

public:
    zip_archive_stream_fd(const char* filepath);
    virtual ~zip_archive_stream_fd();

    virtual void seek_end();
    virtual void seek(size_t pos);

    virtual void read(char* buffer, size_t length) const;

    virtual size_t tell() const;

};

}

#endif
