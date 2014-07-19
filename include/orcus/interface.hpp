/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_INTERFACE_HPP
#define ORCUS_INTERFACE_HPP

#include "env.hpp"

#include <string>

namespace orcus { namespace iface {

struct ORCUS_DLLPUBLIC config
{
    bool debug;

    config();
};

class ORCUS_DLLPUBLIC import_filter
{
    struct impl;
    impl* mp_impl;

public:
    import_filter();
    virtual ~import_filter();
    virtual void read_file(const std::string& filepath) = 0;
    virtual const char* get_name() const = 0;

    void set_config(const config& v);
    const config& get_config() const;
};

class ORCUS_DLLPUBLIC document_dumper
{
public:
    virtual ~document_dumper();
    virtual void dump_flat(const std::string& outdir) const = 0;
    virtual void dump_html(const std::string& outdir) const = 0;
    virtual void dump_check(std::ostream& os) const = 0;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
