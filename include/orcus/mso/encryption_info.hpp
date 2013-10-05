/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_MSO_ENCRYPTION_INFO_HPP__
#define __ORCUS_MSO_ENCRYPTION_INFO_HPP__

#include "../env.hpp"
#include <cstdlib>

namespace orcus { namespace mso {

struct encryption_info_reader_impl;

class ORCUS_DLLPUBLIC encryption_info_reader
{
    encryption_info_reader(const encryption_info_reader&); // disabled
    encryption_info_reader& operator= (const encryption_info_reader&); // disabled

public:
    encryption_info_reader();
    ~encryption_info_reader();

    void read(const char* p, size_t n);

private:
    encryption_info_reader_impl* mp_impl;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
