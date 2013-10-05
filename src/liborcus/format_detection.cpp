/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/format_detection.hpp"

#include <iostream>

#ifdef __ORCUS_ODS
#define ODS_ENABLED 1
#else
#define ODS_ENABLED 0
#endif

#ifdef __ORCUS_XLSX
#define XLSX_ENABLED 1
#else
#define XLSX_ENABLED 0
#endif

#ifdef __ORCUS_GNUMERIC
#define GNUMERIC_ENABLED 1
#else
#define GNUMERIC_ENABLED 0
#endif

#ifdef __ORCUS_XLS_XML
#define XLS_XML_ENABLED 1
#else
#define XLS_XML_ENABLED 0
#endif

#if ODS_ENABLED
#include "orcus/orcus_ods.hpp"
#endif
#if XLSX_ENABLED
#include "orcus/orcus_xlsx.hpp"
#endif
#if GNUMERIC_ENABLED
#include "orcus/orcus_gnumeric.hpp"
#endif
#if XLS_XML_ENABLED
#include "orcus/orcus_xls_xml.hpp"
#endif

using namespace std;

namespace orcus {

format_t detect(const unsigned char* buffer, size_t length)
{
#if ODS_ENABLED
    if (orcus_ods::detect(buffer, length))
        return format_ods;
#endif
#if XLSX_ENABLED
    if (orcus_xlsx::detect(buffer, length))
        return format_xlsx;
#endif
#if GNUMERIC_ENABLED
    if (orcus_gnumeric::detect(buffer, length))
        return format_gnumeric;
#endif
#if XLS_XML_ENABLED
    if (orcus_xls_xml::detect(buffer, length))
        return format_xls_xml;
#endif

    return format_unknown;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
