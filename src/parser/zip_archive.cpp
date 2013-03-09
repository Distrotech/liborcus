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

#include "orcus/zip_archive.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <stdint.h>
#include <cstdio>
#include <sstream>

#include <zlib.h>
#include <zconf.h>

using namespace std;

namespace orcus {

namespace {

class zip_error : public exception
{
    string m_msg;
public:
    zip_error() {}
    zip_error(const string& msg) : m_msg(msg) {}
    virtual ~zip_error() throw() {}

    virtual const char* what() const throw()
    {
        ostringstream os;
        os << "zip error: " << m_msg;
        return os.str().c_str();
    }
};

class zip_inflater
{
    z_stream* m_zlib_cxt;

    zip_inflater(); // disabled
public:
    zip_inflater(z_stream* cxt) : m_zlib_cxt(cxt) {}

    ~zip_inflater()
    {
        if (m_zlib_cxt)
            inflateEnd(m_zlib_cxt);
    }

    bool init()
    {
        int err = inflateInit2(m_zlib_cxt, -MAX_WBITS);
        return err == Z_OK;
    }

    bool inflate()
    {
        int err = ::inflate(m_zlib_cxt, Z_SYNC_FLUSH);
        if (err >= 0 && m_zlib_cxt->msg)
            return false;

        return true;
    }
};

/**
 * Stream doesn't know its size; only its starting offset position within
 * the file stream.
 */
class stream
{
    FILE* m_stream;
    size_t m_pos;
    size_t m_pos_internal;

public:
    stream() : m_stream(NULL), m_pos(0), m_pos_internal(0) {}
    stream(FILE* stream, size_t pos) : m_stream(stream), m_pos(pos), m_pos_internal(0) {}

    string read_string(size_t n)
    {
        if (!n)
            throw zip_error("attempt to read string of zero size.");

        if (fseeko(m_stream, m_pos+m_pos_internal, SEEK_SET))
            throw zip_error();

        vector<char> buf(n+1, '\0');
        size_t bytes_read = fread(&buf[0], 1, n, m_stream);
        if (bytes_read != n)
            throw zip_error("failed to read string.");

        m_pos_internal += n;

        return string(&buf[0]);
    }

    void skip_bytes(size_t n)
    {
        m_pos_internal += n;
    }

    uint32_t read_4bytes()
    {
        if (fseeko(m_stream, m_pos+m_pos_internal, SEEK_SET))
            throw zip_error();

        unsigned char buf[4];
        size_t bytes_read = fread(buf, 1, 4, m_stream);
        if (bytes_read != 4)
            throw zip_error("failed to read 4 bytes.");

        m_pos_internal += 4;

        uint32_t ret = buf[0];
        ret |= (buf[1] << 8);
        ret |= (buf[2] << 16);
        ret |= (buf[3] << 24);

        return ret;
    }

    uint16_t read_2bytes()
    {
        if (fseeko(m_stream, m_pos+m_pos_internal, SEEK_SET))
            throw zip_error();

        unsigned char buf[2];
        size_t bytes_read = fread(buf, 1, 2, m_stream);
        if (bytes_read != 2)
            throw zip_error();

        m_pos_internal += 2;

        uint16_t ret = buf[0];
        ret |= (buf[1] << 8);

        return ret;
    }

    size_t tell() const
    {
        return m_pos + m_pos_internal;
    }
};

struct zip_file_param
{
    enum compress_method_type { stored = 0, deflated = 8 };

    string filename;
    compress_method_type compress_method;
    size_t offset_in_archive;
    size_t size_compressed;
    size_t size_uncompressed;
};

} // anonymous namespace

class zip_archive_impl
{
    typedef std::vector<zip_file_param> file_params_type;

    string m_filepath;
    FILE* m_stream;
    off_t m_stream_size;
    size_t m_central_dir_pos;

    stream m_central_dir_end;

    file_params_type m_file_params;

public:
    zip_archive_impl(const char* path);
    ~zip_archive_impl();

    void open();
    void read_file_entries();
    void dump_file_entry(size_t pos) const;

    size_t get_file_entry_count() const
    {
        return m_file_params.size();
    }

private:

    /**
     * Find the central directory of a zip file, located toward the end before
     * the global comment, and starts with the byte sequence of 0x504b0506.
     */
    size_t seek_central_dir();

    void read_central_dir_end();
};

zip_archive_impl::zip_archive_impl(const char* path) :
    m_filepath(path), m_stream(NULL), m_stream_size(0), m_central_dir_pos(0) {}

zip_archive_impl::~zip_archive_impl()
{
    if (m_stream)
        fclose(m_stream);
}

void zip_archive_impl::open()
{
    cout << "filename: " << m_filepath << endl;
    m_stream = fopen(m_filepath.c_str(), "r");
    if (!m_stream)
        // failed to open the file.
        throw zip_error();

    if (fseeko(m_stream, 0, SEEK_END))
        throw zip_error();

    m_stream_size = ftello(m_stream);
    cout << "stream size: " << m_stream_size << endl;

    size_t central_dir_end_pos = seek_central_dir();
    if (!central_dir_end_pos)
        throw zip_error();

    cout << "central directory position: " << central_dir_end_pos << endl;

    m_central_dir_end = stream(m_stream, central_dir_end_pos);
}

void zip_archive_impl::read_file_entries()
{
    m_file_params.clear();

    // Read the end part of the central directory.
    read_central_dir_end();

    stream central_dir(m_stream, m_central_dir_pos);
    uint32_t magic_num = central_dir.read_4bytes();
    uint16_t v16;
    uint32_t v32;
    while (magic_num == 0x02014b50)
    {
        zip_file_param param;

        cout << "-- file entries" << endl;
        printf("  magic number: 0x%8.8x\n", magic_num);
        v16 = central_dir.read_2bytes();
        cout << "  version made by: " << v16 << endl;
        v16 = central_dir.read_2bytes();
        cout << "  minimum version needed to extract: " << v16 << endl;
        v16 = central_dir.read_2bytes();
        printf("  general purpose bit flag: 0x%4.4x\n", v16);
        v16 = central_dir.read_2bytes();
        cout << "  compression method: " << v16 << " (0=stored, 8=deflated)" << endl;
        param.compress_method = static_cast<zip_file_param::compress_method_type>(v16);

        v16 = central_dir.read_2bytes();
        cout << "  file last modified time: " << v16 << endl;
        v16 = central_dir.read_2bytes();
        cout << "  file last modified date: " << v16 << endl;
        v32 = central_dir.read_4bytes();
        printf("  crc32: 0x%8.8x\n", v32);
        param.size_compressed = central_dir.read_4bytes();
        cout << "  compressed size: " << param.size_compressed << endl;
        param.size_uncompressed = central_dir.read_4bytes();
        cout << "  uncompressed size: " << param.size_uncompressed << endl;
        uint16_t filename_len = central_dir.read_2bytes();
        cout << "  file name length: " << filename_len << endl;
        uint16_t extra_field_len = central_dir.read_2bytes();
        cout << "  extra field length: " << extra_field_len << endl;
        uint16_t file_comment_len = central_dir.read_2bytes();
        cout << "  file comment length: " << file_comment_len << endl;
        v16 = central_dir.read_2bytes();
        cout << "  disk number where file starts: " << v16 << endl;
        v16 = central_dir.read_2bytes();
        printf("  internal file attributes: 0x%4.4x\n", v16);
        v32 = central_dir.read_4bytes();
        printf("  external file attributes: 0x%8.8x\n", v32);
        param.offset_in_archive = central_dir.read_4bytes();
        cout << "  relative offset of local file header: " << param.offset_in_archive << endl;

        if (filename_len)
        {
            param.filename = central_dir.read_string(filename_len);
            cout << "  filename: '" << param.filename << "'" << endl;
        }

        if (extra_field_len)
        {
            // Ignore extra field for now.
            central_dir.skip_bytes(extra_field_len);
        }

        if (file_comment_len)
        {
            // Ignore file comment for now.
            central_dir.skip_bytes(file_comment_len);
        }

        magic_num = central_dir.read_4bytes(); // magic number for the next entry.

        m_file_params.push_back(param);

        cout << "--" << endl;
    }
}

void zip_archive_impl::dump_file_entry(size_t pos) const
{
    if (pos >= m_file_params.size())
        throw zip_error("invalid file entry index.");

    const zip_file_param& param = m_file_params[pos];
    cout << "-- filename: " << param.filename << endl;

    stream file_header(m_stream, param.offset_in_archive);
    uint32_t v32 = file_header.read_4bytes();
    printf("  header signature: 0x%8.8x\n", v32);
    uint16_t v16 = file_header.read_2bytes();
    cout << "  version needed to extract: " << v16 << endl;
    v16 = file_header.read_2bytes();
    printf("  general purpose bit flag: 0x%4.4x\n", v16);
    v16 = file_header.read_2bytes();
    cout << "  compression method: " << v16 << endl;
    v16 = file_header.read_2bytes();
    cout << "  file last modified time: " << v16 << endl;
    v16 = file_header.read_2bytes();
    cout << "  file last modified date: " << v16 << endl;
    v32 = file_header.read_4bytes();
    printf("  crc32: 0x%8.8x\n", v32);
    v32 = file_header.read_4bytes();
    cout << "  compressed size: " << v32 << endl;
    v32 = file_header.read_4bytes();
    cout << "  uncompressed size: " << v32 << endl;
    uint16_t filename_len = file_header.read_2bytes();
    cout << "  filename length: " << filename_len << endl;
    uint16_t extra_field_len = file_header.read_2bytes();
    cout << "  extra field length: " << extra_field_len << endl;
    if (filename_len)
    {
        string filename = file_header.read_string(filename_len);
        cout << "  filename: '" << filename << "'" << endl;
    }

    if (extra_field_len)
    {
        // Ignore extra field.
        file_header.skip_bytes(extra_field_len);
    }

    // Header followed by the actual data bytes.

    if (fseeko(m_stream, file_header.tell(), SEEK_SET))
        // Failed to seek in the stream.
        return;

    vector<unsigned char> raw_buf(param.size_compressed+1, 0);
    size_t size_read = fread(&raw_buf[0], 1, param.size_compressed, m_stream);
    if (size_read != param.size_compressed)
        throw zip_error("actual size read doesn't match what was expected.");

    cout << "-- data section" << endl;
    switch (param.compress_method)
    {
        case zip_file_param::stored:
            // Not compressed at all.
            cout << &raw_buf[0] << endl;
        break;
        case zip_file_param::deflated:
        {
            // deflate compression
            z_stream zlib_cxt;
            zlib_cxt.total_out = 0;
            zlib_cxt.zalloc = 0;
            zlib_cxt.zfree = 0;
            zlib_cxt.opaque = 0;
            zlib_cxt.next_in = &raw_buf[0];
            zlib_cxt.avail_in = param.size_compressed;

            size_t zip_buf_size = param.size_uncompressed;

            vector<unsigned char> zip_buf(zip_buf_size+1, 0);

            zlib_cxt.next_out = &zip_buf[0];
            zlib_cxt.avail_out = zip_buf_size;

            zip_inflater inflater(&zlib_cxt);
            if (!inflater.init())
                break;

            if (!inflater.inflate())
                throw zip_error("error during inflate.");

            cout << &zip_buf[0] << endl;
        }
        break;
        default:
            ;
    }

    cout << "--" << endl;
}

size_t zip_archive_impl::seek_central_dir()
{
    cout << "searching for the central directory position..." << endl;

    // Search for the position of 0x06054b50 (read in little endian order - so
    // it's 0x50, 0x4b, 0x05, 0x06 in this order) somewhere near the end of
    // the stream.

    unsigned char magic[] = { 0x06, 0x05, 0x4b, 0x50 };
    size_t n_magic = 4;

    off_t max_comment_size = 0xffff;

    size_t buf_size = 22 + max_comment_size; // central directory size is 22 + n (n maxing at 0xffff).
    vector<unsigned char> buf(buf_size);

    // Read stream backward and try to find the magic number.

    size_t read_end_pos = m_stream_size;
    while (true)
    {
        if (read_end_pos < buf.size())
        {
            // Last segment to read.
            cout << "last segment to read" << endl;
            buf.resize(read_end_pos);
        }

        size_t read_pos = read_end_pos - buf.size();
        cout << "read pos: " << read_pos << endl;
        if (fseeko(m_stream, read_pos, SEEK_SET))
            throw zip_error();

        size_t bytes_read = fread(&buf[0], 1, buf.size(), m_stream);
        if (bytes_read != buf.size())
            throw zip_error();

        // Search this byte segment for the magic number.
        vector<unsigned char>::reverse_iterator i = buf.rbegin(), ie = buf.rend();
        size_t magic_pos = 0;
        for (; i != ie; ++i)
        {
            // 06 05 4b 50
            if (*i == magic[magic_pos])
            {
                ++magic_pos;
                if (magic_pos == n_magic)
                {
                    // magic number is found.
                    size_t dist = distance(buf.rbegin(), i) + 1;
                    size_t pos = read_end_pos - dist;
                    return pos;
                }
            }
            else
                magic_pos = 0;
        }

        read_end_pos -= buf.size();
    }

    return 0;
}

void zip_archive_impl::read_central_dir_end()
{
    cout << "-- central directory content" << endl;

    uint32_t v32 = m_central_dir_end.read_4bytes();
    printf("  magic number: 0x%8.8x\n", v32);

    uint16_t v16 = m_central_dir_end.read_2bytes();
    printf("  number of this disk: %d\n", v16);
    v16 = m_central_dir_end.read_2bytes();
    printf("  disk where central directory starts: %d\n", v16);
    v16 = m_central_dir_end.read_2bytes();
    printf("  number of central directory records on this disk: %d\n", v16);
    v16 = m_central_dir_end.read_2bytes();
    printf("  total number of central directory records: %d\n", v16);
    v32 = m_central_dir_end.read_4bytes();
    printf("  size of central directory: %d\n", v32);
    v32 = m_central_dir_end.read_4bytes();
    printf("  offset of start of central directory, relative to start of archive: %d\n", v32);
    m_central_dir_pos = v32;

    v16 = m_central_dir_end.read_2bytes();
    printf("  comment length: %d\n", v16);

    cout << "--" << endl;
}

zip_archive::zip_archive(const char* filepath) :
    mp_impl(new zip_archive_impl(filepath))
{
}

zip_archive::~zip_archive()
{
    delete mp_impl;
}

void zip_archive::open()
{
    mp_impl->open();
}

void zip_archive::read_file_entries()
{
    mp_impl->read_file_entries();
}

void zip_archive::dump_file_entry(size_t pos) const
{
    mp_impl->dump_file_entry(pos);
}

size_t zip_archive::get_file_entry_count() const
{
    return mp_impl->get_file_entry_count();
}

}
