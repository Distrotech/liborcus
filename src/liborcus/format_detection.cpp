/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

    return format_unknown;
}

}
