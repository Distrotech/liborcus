/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_ods.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/zip_archive.hpp"
#include "orcus/zip_archive_stream.hpp"

#include "xml_stream_parser.hpp"
#include "ods_content_xml_handler.hpp"
#include "odf_tokens.hpp"
#include "odf_namespace_types.hpp"
#include "session_context.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

#include <boost/scoped_ptr.hpp>

using namespace std;

namespace orcus {

struct orcus_ods_impl
{
    xmlns_repository m_ns_repo;
    session_context m_cxt;
    spreadsheet::iface::import_factory* mp_factory;

    orcus_ods_impl(spreadsheet::iface::import_factory* im_factory) :
        mp_factory(im_factory) {}
};

orcus_ods::orcus_ods(spreadsheet::iface::import_factory* factory) :
    mp_impl(new orcus_ods_impl(factory))
{
    mp_impl->m_ns_repo.add_predefined_values(NS_odf_all);
}

orcus_ods::~orcus_ods()
{
    delete mp_impl;
}

void orcus_ods::list_content(const zip_archive& archive)
{
    size_t num = archive.get_file_entry_count();
    cout << "number of files this archive contains: " << num << endl;

    for (size_t i = 0; i < num; ++i)
    {
        pstring filename = archive.get_file_entry_name(i);
        if (filename.empty())
            cout << "(empty)" << endl;
        else
            cout << filename << endl;
    }
}

void orcus_ods::read_content(const zip_archive& archive)
{
    vector<unsigned char> buf;
    if (!archive.read_file_entry("content.xml", buf))
    {
        cout << "failed to get stat on content.xml" << endl;
        return;
    }

    read_content_xml(&buf[0], buf.size());
}

void orcus_ods::read_content_xml(const unsigned char* p, size_t size)
{
    xml_stream_parser parser(mp_impl->m_ns_repo, odf_tokens, reinterpret_cast<const char*>(p), size, "content.xml");
    ::boost::scoped_ptr<ods_content_xml_handler> handler(
        new ods_content_xml_handler(mp_impl->m_cxt, odf_tokens, mp_impl->mp_factory));
    parser.set_handler(handler.get());
    parser.parse();
}

bool orcus_ods::detect(const unsigned char* blob, size_t size)
{
    zip_archive_stream_blob stream(blob, size);
    zip_archive archive(&stream);
    try
    {
        archive.load();
    }
    catch (const zip_error&)
    {
        // Not a valid zip archive.
        return false;
    }

    vector<unsigned char> buf;
    if (!archive.read_file_entry("mimetype", buf))
        // Failed to read 'mimetype' entry.
        return false;

    if (buf.empty())
        // mimetype is empty.
        return false;

    const char* mimetype = "application/vnd.oasis.opendocument.spreadsheet";
    size_t n = strlen(mimetype);
    if (buf.size() < n)
        return false;

    if (strncmp(mimetype, reinterpret_cast<const char*>(&buf[0]), n))
        // The mimetype content differs.
        return false;

    return true;
}

void orcus_ods::read_file(const string& filepath)
{
    zip_archive_stream_fd stream(filepath.c_str());
    zip_archive archive(&stream);
    archive.load();
    list_content(archive);
    read_content(archive);

    mp_impl->mp_factory->finalize();
}

const char* orcus_ods::get_name() const
{
    static const char* name = "ods";
    return name;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
