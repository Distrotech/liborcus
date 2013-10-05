/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
