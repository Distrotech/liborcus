/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_namespace_types.hpp"

namespace orcus {

const xmlns_id_t NS_ooxml_r    = "http://schemas.openxmlformats.org/officeDocument/2006/relationships";
const xmlns_id_t NS_ooxml_xlsx = "http://schemas.openxmlformats.org/spreadsheetml/2006/main";

const xmlns_id_t NS_opc_ct  = "http://schemas.openxmlformats.org/package/2006/content-types";
const xmlns_id_t NS_opc_rel = "http://schemas.openxmlformats.org/package/2006/relationships";

const xmlns_id_t NS_mc = "http://schemas.openxmlformats.org/markup-compatibility/2006";

namespace {

xmlns_id_t ooxml_ns[] = {
    NS_ooxml_r,
    NS_ooxml_xlsx,
    NULL
};

xmlns_id_t opc_ns[] = {
    NS_opc_ct,
    NS_opc_rel,
    NULL
};

xmlns_id_t misc_ns[] = {
    NS_mc,
    NULL
};

}

const xmlns_id_t* NS_ooxml_all = ooxml_ns;
const xmlns_id_t* NS_opc_all = opc_ns;
const xmlns_id_t* NS_misc_all = misc_ns;

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
