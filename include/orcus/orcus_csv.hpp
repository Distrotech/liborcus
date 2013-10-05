/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ORCUS_CSV_HPP
#define ORCUS_ORCUS_CSV_HPP

#include "interface.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_factory;
}}

class ORCUS_DLLPUBLIC orcus_csv : public iface::import_filter
{
    orcus_csv(const orcus_csv&); // disabled
    orcus_csv& operator=(const orcus_csv&); // disabled

public:
    orcus_csv(spreadsheet::iface::import_factory* factory);

    virtual void read_file(const std::string& filepath);
    virtual const char* get_name() const;

private:
    void parse(const std::string& strm);

private:
    spreadsheet::iface::import_factory* mp_factory;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
