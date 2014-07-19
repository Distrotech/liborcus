/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_gnumeric.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/stream.hpp"

#include "xml_stream_parser.hpp"
#include "gnumeric_handler.hpp"
#include "gnumeric_tokens.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_detection_handler.hpp"
#include "session_context.hpp"
#include "detection_result.hpp"

#define ORCUS_DEBUG_GNUMERIC 0
#define BOOST_IOSTREAMS_NO_LIB 1

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace std;

namespace orcus {

namespace {

bool decompress_gzip(const char* buffer, size_t size, string& decompressed)
{
    string buf;

    try
    {
        boost::iostreams::filtering_ostream os;
        os.push(boost::iostreams::gzip_decompressor());
        os.push(boost::iostreams::back_inserter(buf));
        boost::iostreams::write(os, buffer, size);
        os.flush();
    }
    catch (const exception&)
    {
        return false;
    }

    buf.swap(decompressed);
    return true;
}

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
    xml_stream_parser parser(get_config(), mp_impl->m_ns_repo, gnumeric_tokens, p, size);
    ::boost::scoped_ptr<gnumeric_content_xml_handler> handler(
        new gnumeric_content_xml_handler(mp_impl->m_cxt, gnumeric_tokens, mp_impl->mp_factory));
    parser.set_handler(handler.get());
    parser.parse();
}

bool orcus_gnumeric::detect(const unsigned char* buffer, size_t size)
{
    // Detect gnumeric format that's already in memory.

    string decompressed;
    if (!decompress_gzip(reinterpret_cast<const char*>(buffer), size, decompressed))
        return false;

    if (decompressed.empty())
        return false;

    // Parse this xml stream for detection.
    iface::config opt;
    xmlns_repository ns_repo;
    ns_repo.add_predefined_values(NS_gnumeric_all);
    session_context cxt;
    xml_stream_parser parser(opt, ns_repo, gnumeric_tokens, &decompressed[0], decompressed.size());
    gnumeric_detection_handler handler(cxt, gnumeric_tokens);
    parser.set_handler(&handler);

    try
    {
        parser.parse();
    }
    catch (const detection_result& res)
    {
        return res.get_result();
    }
    catch (...) {}

    return false;
}

void orcus_gnumeric::read_file(const string& filepath)
{
#if ORCUS_DEBUG_GNUMERIC
    cout << "reading " << filepath << endl;
#endif

    string strm;
    load_file_content(filepath.c_str(), strm);
    if (strm.empty())
        return;

    string file_content;
    if (!decompress_gzip(&strm[0], strm.size(), file_content))
        return;

    read_content_xml(file_content.c_str(), file_content.length());

    mp_impl->mp_factory->finalize();
}

const char* orcus_gnumeric::get_name() const
{
    static const char* name = "gnumeric";
    return name;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
