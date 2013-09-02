/*************************************************************************
 *
 * Copyright (c) 2010-2013 Kohei Yoshida
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

#include "orcus/orcus_gnumeric.hpp"
#include "orcus/xml_namespace.hpp"

#include "xml_stream_parser.hpp"
#include "gnumeric_handler.hpp"
#include "gnumeric_tokens.hpp"
#include "gnumeric_namespace_types.hpp"
#include "session_context.hpp"

#include <zlib.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace std;

#define BUFFER_LENGTH 0x2000

namespace orcus {

namespace {

class gzfile_scope
{
    gzFile m_file;
public:
    gzfile_scope(const char* fpath) : m_file(gzopen(fpath, "rb")) {}
    ~gzfile_scope()
    {
        if (m_file)
            gzclose(m_file);
    }

    gzFile get() { return m_file; }
};

}

struct orcus_gnumeric_impl
{
    xmlns_repository m_ns_repo;
    session_context m_cxt;
    spreadsheet::iface::import_factory* mp_factory;

    orcus_gnumeric_impl(spreadsheet::iface::import_factory* im_factory) :
        mp_factory(im_factory) {}
};

orcus_gnumeric::orcus_gnumeric(spreadsheet::iface::import_factory* factory) :
    mp_impl(new orcus_gnumeric_impl(factory))
{
    mp_impl->m_ns_repo.add_predefined_values(NS_gnumeric_all);
}

orcus_gnumeric::~orcus_gnumeric()
{
    delete mp_impl;
}

void orcus_gnumeric::read_content_xml(const char* p, size_t size)
{
    xml_stream_parser parser(mp_impl->m_ns_repo, gnumeric_tokens, p, size, "content.xml");
    ::boost::scoped_ptr<gnumeric_content_xml_handler> handler(
        new gnumeric_content_xml_handler(mp_impl->m_cxt, gnumeric_tokens, mp_impl->mp_factory));
    parser.set_handler(handler.get());
    parser.parse();
}

bool orcus_gnumeric::detect(const unsigned char* buffer, size_t size)
{
    // Detect gnumeric format that's already in memory.

    try
    {
        // First, decompress the gzipped stream.
        vector<char> decompressed;
        boost::iostreams::filtering_ostream os;
        os.push(boost::iostreams::gzip_decompressor());
        os.push(boost::iostreams::back_inserter(decompressed));
        boost::iostreams::write(os, reinterpret_cast<const char*>(buffer), size);
        os.flush();

        for (size_t i = 0; i < decompressed.size(); ++i)
            cout << decompressed[i];
        cout << endl;

        // TODO: parse this xml stream for detection.
    }
    catch (const std::exception&) {}

    return false;
}

void orcus_gnumeric::read_file(const char *fpath)
{
    cout << "reading " << fpath << endl;

    gzfile_scope file_scope(fpath);
    gzFile file = file_scope.get();

    if (!file)
        return;

    std::string file_content;

    while (true)
    {
        char buffer[BUFFER_LENGTH];
        int read_characters = gzread(file, buffer, BUFFER_LENGTH);
        if (read_characters < 0)
        {
            std::cout << "Read error" << std::endl;
            break;
        }

        file_content.append(buffer, read_characters);
        if (read_characters < BUFFER_LENGTH)
        {
            if (gzeof(file))
                break;
            else
            {
                const char *error;
                int err;
                error = gzerror(file, &err);
                std::cout << "error: " << error << std::endl;
            }
        }
    }

    read_content_xml(file_content.c_str(), file_content.length());

    mp_impl->mp_factory->finalize();
}

}
