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
#include "orcus/zip_archive_stream.hpp"
#include "orcus/string_pool.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <stdint.h>
#include <cstdio>
#include <sstream>

#include <zlib.h>
#include <zconf.h>

#define ORCUS_DEBUG_ZIP_ARCHIVE 0

using namespace std;

namespace orcus {

zip_error::zip_error() {}
zip_error::zip_error(const string& msg) : m_msg(msg) {}
zip_error::~zip_error() throw() {}

const char* zip_error::what() const throw()
{
    ostringstream os;
    os << "zip error: " << m_msg;
    return os.str().c_str();
}

namespace {

struct zip_file_param
{
    enum compress_method_type { stored = 0, deflated = 8 };

    pstring filename;
    compress_method_type compress_method;
    size_t offset_file_header;
    size_t offset_data_stream;
    size_t size_compressed;
    size_t size_uncompressed;

    uint16_t version_made_by;
    uint16_t minimum_version_needed;
    uint16_t flags;
    uint16_t last_modified_time;
    uint16_t last_modified_date;

    uint16_t filename_length;
    uint16_t extra_field_length;
    uint16_t comment_length;

    uint16_t disk_id_where_file_starts;
    uint16_t file_attributes_internal;
    uint32_t file_attributes_external;

    uint32_t crc32;
};

class zip_inflater
{
    z_stream m_zlib_cxt;

    zip_inflater(); // disabled
public:
    zip_inflater(vector<unsigned char>& raw_buf, vector<unsigned char>& dest_buf, const zip_file_param& param)
    {
        m_zlib_cxt.total_out = 0;
        m_zlib_cxt.zalloc = 0;
        m_zlib_cxt.zfree = 0;
        m_zlib_cxt.opaque = 0;
        m_zlib_cxt.next_in = static_cast<Bytef*>(&raw_buf[0]);
        m_zlib_cxt.avail_in = param.size_compressed;

        m_zlib_cxt.next_out = static_cast<Bytef*>(&dest_buf[0]);
        m_zlib_cxt.avail_out = param.size_uncompressed;
    }

    ~zip_inflater()
    {
        inflateEnd(&m_zlib_cxt);
    }

    bool init()
    {
        int err = inflateInit2(&m_zlib_cxt, -MAX_WBITS);
        return err == Z_OK;
    }

    bool inflate()
    {
        int err = ::inflate(&m_zlib_cxt, Z_SYNC_FLUSH);
        if (err >= 0 && m_zlib_cxt.msg)
            return false;

        return true;
    }
};

/**
 * Stream doesn't know its size; only its starting offset position within
 * the file stream.
 */
class zip_stream_parser
{
    zip_archive_stream* m_stream;
    size_t m_pos;
    size_t m_pos_internal;

    void read_string_to_buffer(size_t n, vector<unsigned char>& buf)
    {
        if (!n)
            throw zip_error("attempt to read string of zero size.");

        m_stream->seek(m_pos+m_pos_internal);
        m_stream->read(&buf[0], n);
        m_pos_internal += n;
    }

public:
    zip_stream_parser() : m_stream(NULL), m_pos(0), m_pos_internal(0) {}
    zip_stream_parser(zip_archive_stream* stream, size_t pos) : m_stream(stream), m_pos(pos), m_pos_internal(0) {}

    string read_string(size_t n)
    {
        vector<unsigned char> buf(n+1, '\0');
        read_string_to_buffer(n, buf);
        return string(reinterpret_cast<const char*>(&buf[0]));
    }

    pstring read_string(size_t n, string_pool& pool)
    {
        vector<unsigned char> buf(n+1, '\0');
        read_string_to_buffer(n, buf);
        return pool.intern(reinterpret_cast<const char*>(&buf[0]), n).first;
    }

    void skip_bytes(size_t n)
    {
        m_pos_internal += n;
    }

    uint32_t read_4bytes()
    {
        m_stream->seek(m_pos+m_pos_internal);
        unsigned char buf[4];
        m_stream->read(&buf[0], 4);
        m_pos_internal += 4;

        uint32_t ret = buf[0];
        ret |= (buf[1] << 8);
        ret |= (buf[2] << 16);
        ret |= (buf[3] << 24);

        return ret;
    }

    uint16_t read_2bytes()
    {
        m_stream->seek(m_pos+m_pos_internal);
        unsigned char buf[2];
        m_stream->read(&buf[0], 2);
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

/**
 * Content of the end part of the central directory.
 */
struct central_dir_end
{
    uint32_t magic_number;
    uint16_t this_disk_id;
    uint16_t central_dir_disk_id;
    uint16_t num_central_dir_records_local;
    uint16_t num_celtral_dir_records_total;
    uint32_t size_central_dir;
    size_t central_dir_pos;
    uint16_t comment_length;
};

} // anonymous namespace

class zip_archive_impl
{
    typedef std::vector<zip_file_param> file_params_type;
    typedef boost::unordered_map<pstring, size_t, pstring::hash> filename_map_type;

    string_pool m_pool;
    zip_archive_stream* m_stream;
    off_t m_stream_size;
    size_t m_central_dir_pos;

    zip_stream_parser m_central_dir_end;

    file_params_type m_file_params;
    filename_map_type m_filenames;

public:
    zip_archive_impl(zip_archive_stream* stream);
    ~zip_archive_impl();

    void load();
    void dump_file_entry(size_t pos) const;
    void dump_file_entry(const char* entry_name) const;
    pstring get_file_entry_name(size_t pos) const;

    size_t get_file_entry_count() const
    {
        return m_file_params.size();
    }

    bool read_file_entry(const pstring& entry_name, vector<unsigned char>& buf) const;

private:

    /**
     * Find the central directory of a zip file, located toward the end before
     * the global comment, and starts with the byte sequence of 0x504b0506.
     */
    size_t seek_central_dir();

    void read_central_dir_end();
    void read_file_entries();
};

zip_archive_impl::zip_archive_impl(zip_archive_stream* stream) :
    m_stream(stream), m_stream_size(0), m_central_dir_pos(0)
{
    if (!m_stream)
        zip_error("null stream is not allowed.");

    m_stream_size = m_stream->size();
}

zip_archive_impl::~zip_archive_impl()
{
}

void zip_archive_impl::load()
{
    size_t central_dir_end_pos = seek_central_dir();
    if (!central_dir_end_pos)
        throw zip_error();

    m_central_dir_end = zip_stream_parser(m_stream, central_dir_end_pos);

    // Read the end part of the central directory.
    read_central_dir_end();

    // Read file entries that are in the front part of the central directory.
    read_file_entries();
}

void zip_archive_impl::read_file_entries()
{
    m_file_params.clear();

    zip_stream_parser central_dir(m_stream, m_central_dir_pos);
    uint32_t magic_num = central_dir.read_4bytes();

    while (magic_num == 0x02014b50)
    {
        zip_file_param param;

        param.version_made_by = central_dir.read_2bytes();
        param.minimum_version_needed = central_dir.read_2bytes();
        param.flags = central_dir.read_2bytes();
        param.compress_method =
            static_cast<zip_file_param::compress_method_type>(central_dir.read_2bytes());

        param.last_modified_time = central_dir.read_2bytes();
        param.last_modified_date = central_dir.read_2bytes();
        param.crc32 = central_dir.read_4bytes();
        param.size_compressed = central_dir.read_4bytes();
        param.size_uncompressed = central_dir.read_4bytes();
        param.filename_length = central_dir.read_2bytes();
        param.extra_field_length = central_dir.read_2bytes();
        param.comment_length = central_dir.read_2bytes();
        param.disk_id_where_file_starts = central_dir.read_2bytes();
        param.file_attributes_internal = central_dir.read_2bytes();
        param.file_attributes_external = central_dir.read_4bytes();
        param.offset_file_header = central_dir.read_4bytes();

        if (param.filename_length)
            param.filename = central_dir.read_string(param.filename_length, m_pool);

        if (param.extra_field_length)
            // Ignore extra field for now.
            central_dir.skip_bytes(param.extra_field_length);

        if (param.comment_length)
            // Ignore file comment for now.
            central_dir.skip_bytes(param.comment_length);

        magic_num = central_dir.read_4bytes(); // magic number for the next entry.

        m_file_params.push_back(param);
        m_filenames.insert(filename_map_type::value_type(param.filename, m_file_params.size()-1));

#if ORCUS_DEBUG_ZIP_ARCHIVE
        cout << "-- file entries" << endl;
        printf( "  magic number: 0x%8.8x\n", magic_num);
        cout << "  version made by: " << param.version_made_by << endl;
        cout << "  minimum version needed to extract: " << param.minimum_version_needed << endl;
        printf( "  general purpose bit flag: 0x%4.4x\n", param.flags);
        cout << "  compression method: " << param.compress_method << " (0=stored, 8=deflated)" << endl;
        cout << "  file last modified time: " << param.last_modified_time << endl;
        cout << "  file last modified date: " << param.last_modified_date << endl;
        printf( "  crc32: 0x%8.8x\n", param.crc32);
        cout << "  compressed size: " << param.size_compressed << endl;
        cout << "  uncompressed size: " << param.size_uncompressed << endl;
        cout << "  file name length: " << param.filename_length << endl;
        cout << "  extra field length: " << param.extra_field_length << endl;
        cout << "  file comment length: " << param.comment_length << endl;
        cout << "  disk number where file starts: " << param.disk_id_where_file_starts << endl;
        printf( "  internal file attributes: 0x%4.4x\n", param.file_attributes_internal);
        printf( "  external file attributes: 0x%8.8x\n", param.file_attributes_external);
        cout << "  relative offset of local file header: " << param.offset_file_header << endl;

        if (param.filename_length)
            cout << "  filename: '" << param.filename << "'" << endl;

        cout << "--" << endl;
#endif
    }
}

void zip_archive_impl::dump_file_entry(size_t pos) const
{
    if (pos >= m_file_params.size())
        throw zip_error("invalid file entry index.");

    const zip_file_param& param = m_file_params[pos];
    cout << "-- filename: " << param.filename << endl;

    zip_stream_parser file_header(m_stream, param.offset_file_header);
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
    size_t filename_len = file_header.read_2bytes();
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

    m_stream->seek(file_header.tell());

    vector<unsigned char> buf;
    if (read_file_entry(param.filename, buf))
    {
        cout << "-- data section" << endl;
        cout << &buf[0] << endl;
        cout << "--" << endl;
    }
}

void zip_archive_impl::dump_file_entry(const char* entry_name) const
{
    pstring name(entry_name);
    filename_map_type::const_iterator it = m_filenames.find(name);
    if (it == m_filenames.end())
    {
        // entry name not found.
        cout << "file entry '" << entry_name << "' not found." << endl;
        return;
    }

    dump_file_entry(it->second);
}

pstring zip_archive_impl::get_file_entry_name(size_t pos) const
{
    if (pos >= m_file_params.size())
        return NULL;

    return m_file_params[pos].filename;
}

bool zip_archive_impl::read_file_entry(const pstring& entry_name, vector<unsigned char>& buf) const
{
    pstring name(entry_name);
    filename_map_type::const_iterator it = m_filenames.find(name);
    if (it == m_filenames.end())
        // entry name not found.
        return false;

    size_t index = it->second;
    if (index >= m_file_params.size())
        // entry index is out of bound.
        return false;

    const zip_file_param& param = m_file_params[index];

    // Skip the file header section.
    zip_stream_parser file_header(m_stream, param.offset_file_header);
    file_header.skip_bytes(4);
    file_header.skip_bytes(2);
    file_header.skip_bytes(2);
    file_header.skip_bytes(2);
    file_header.skip_bytes(2);
    file_header.skip_bytes(2);
    file_header.skip_bytes(4);
    file_header.skip_bytes(4);
    file_header.skip_bytes(4);
    uint16_t filename_len = file_header.read_2bytes();
    uint16_t extra_field_len = file_header.read_2bytes();
    file_header.skip_bytes(filename_len);
    file_header.skip_bytes(extra_field_len);

    // Data section is immediately followed by the header section.
    m_stream->seek(file_header.tell());

    vector<unsigned char> raw_buf(param.size_compressed+1, 0);
    m_stream->read(&raw_buf[0], param.size_compressed);

    switch (param.compress_method)
    {
        case zip_file_param::stored:
            // Not compressed at all.
            buf.swap(raw_buf);
            return true;
        case zip_file_param::deflated:
        {
            // deflate compression
            vector<unsigned char> zip_buf(param.size_uncompressed+1, 0); // null-terminated
            zip_inflater inflater(raw_buf, zip_buf, param);
            if (!inflater.init())
                break;

            if (!inflater.inflate())
                throw zip_error("error during inflate.");

            buf.swap(zip_buf);
            return true;
        }
        default:
            ;
    }

    return false;
}

size_t zip_archive_impl::seek_central_dir()
{
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
            // Last segment to read.
            buf.resize(read_end_pos);

        size_t read_pos = read_end_pos - buf.size();
        m_stream->seek(read_pos);
        m_stream->read(&buf[0], buf.size());

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
    central_dir_end content;
    content.magic_number = m_central_dir_end.read_4bytes();
    content.this_disk_id = m_central_dir_end.read_2bytes();
    content.central_dir_disk_id = m_central_dir_end.read_2bytes();
    content.num_central_dir_records_local = m_central_dir_end.read_2bytes();
    content.num_celtral_dir_records_total = m_central_dir_end.read_2bytes();
    content.size_central_dir = m_central_dir_end.read_4bytes();
    content.central_dir_pos = m_central_dir_end.read_4bytes();
    m_central_dir_pos = content.central_dir_pos;

    content.comment_length = m_central_dir_end.read_2bytes();

#if ORCUS_DEBUG_ZIP_ARCHIVE
    cout << "-- central directory content" << endl;
    printf("  magic number: 0x%8.8x\n", content.magic_number);
    cout << "  number of this disk: " << content.this_disk_id << endl;
    cout << "  disk where central directory starts: " << content.central_dir_disk_id << endl;
    cout << "  number of central directory records on this disk: " << content.num_central_dir_records_local << endl;
    cout << "  total number of central directory records: " << content.num_celtral_dir_records_total << endl;
    cout << "  size of central directory: " << content.size_central_dir << endl;
    cout << "  offset of start of central directory, relative to start of archive: " << content.central_dir_pos << endl;
    cout << "  comment length: " << content.comment_length << endl;
    cout << "--" << endl;
#endif
}

zip_archive::zip_archive(zip_archive_stream* stream) :
    mp_impl(new zip_archive_impl(stream))
{
}

zip_archive::~zip_archive()
{
    delete mp_impl;
}

void zip_archive::load()
{
    mp_impl->load();
}

void zip_archive::dump_file_entry(size_t index) const
{
    mp_impl->dump_file_entry(index);
}

pstring zip_archive::get_file_entry_name(size_t index) const
{
    return mp_impl->get_file_entry_name(index);
}

void zip_archive::dump_file_entry(const char* entry_name) const
{
    mp_impl->dump_file_entry(entry_name);
}

size_t zip_archive::get_file_entry_count() const
{
    return mp_impl->get_file_entry_count();
}

bool zip_archive::read_file_entry(const pstring& entry_name, vector<unsigned char>& buf) const
{
    return mp_impl->read_file_entry(entry_name, buf);
}

}
