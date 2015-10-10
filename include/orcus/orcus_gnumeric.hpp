/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ORCUS_GNUMERIC_HPP
#define ORCUS_ORCUS_GNUMERIC_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct orcus_gnumeric_impl;

class ORCUS_DLLPUBLIC orcus_gnumeric : public iface::import_filter
{
    orcus_gnumeric(const orcus_gnumeric&); // disabled
public:
    orcus_gnumeric(spreadsheet::iface::import_factory* factory);
    ~orcus_gnumeric();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);

    virtual void read_stream(const char* content, size_t len);

    virtual const char* get_name() const;

private:
    void read_content_xml(const char* p, size_t size);

private:
    orcus_gnumeric_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
