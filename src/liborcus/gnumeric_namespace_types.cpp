/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include "gnumeric_namespace_types.hpp"
#include "odf_namespace_types.hpp"

namespace orcus {

const xmlns_id_t NS_gnumeric_dc = "http://purl.org/dc/elements/1.1/";
const xmlns_id_t NS_gnumeric_gnm = "http://www.gnumeric.org/v10.dtd";
const xmlns_id_t NS_gnumeric_ooo = "http://openoffice.org/2004/office";
const xmlns_id_t NS_gnumeric_xlink = "http://www.w3.org/1999/xlink";
const xmlns_id_t NS_gnumeric_xsi = "http://www.w3.org/2001/XMLSchema-instance";

namespace {

xmlns_id_t gnumeric_ns[] = {
    NS_gnumeric_dc,
    NS_gnumeric_gnm,
    NS_gnumeric_ooo,
    NS_gnumeric_xlink,
    NS_gnumeric_xsi,

    NS_odf_meta,
    NS_odf_office,
    NULL
};

}

const xmlns_id_t* NS_gnumeric_all = gnumeric_ns;

}
