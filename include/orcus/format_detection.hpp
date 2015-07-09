/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_FORMAT_DETECTION_HPP
#define ORCUS_FORMAT_DETECTION_HPP

#include "env.hpp"

#include <cstdlib>

namespace orcus {

enum class format_t
{
    unknown = 0,
    ods,
    xlsx,
    gnumeric,
    xls_xml,
    csv
};

ORCUS_DLLPUBLIC format_t detect(const unsigned char* buffer, size_t length);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
