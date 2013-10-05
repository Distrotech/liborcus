/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ORCUS_XLS_XML_HPP
#define ORCUS_ORCUS_XLS_XML_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface { class import_factory; }}

struct orcus_xls_xml_impl;

class ORCUS_DLLPUBLIC orcus_xls_xml : public iface::import_filter
{
    orcus_xls_xml(const orcus_xls_xml&); // disabled
    orcus_xls_xml& operator= (const orcus_xls_xml&); // disabled

public:
    orcus_xls_xml(spreadsheet::iface::import_factory* factory);
    ~orcus_xls_xml();

    static bool detect(const unsigned char* blob, size_t size);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:
    orcus_xls_xml_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
