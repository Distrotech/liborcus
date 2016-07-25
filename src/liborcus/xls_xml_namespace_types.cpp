/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xls_xml_namespace_types.hpp"

namespace orcus {

const xmlns_id_t NS_xls_xml_ss = "urn:schemas-microsoft-com:office:spreadsheet";
const xmlns_id_t NS_xls_xml_o ="urn:schemas-microsoft-com:office:office";
const xmlns_id_t NS_xls_xml_x ="urn:schemas-microsoft-com:office:excel";
const xmlns_id_t NS_xls_xml_html ="http://www.w3.org/TR/REC-html40";

namespace {

xmlns_id_t xls_xml_ns[] = {
    NS_xls_xml_ss,
    NS_xls_xml_o,
    NS_xls_xml_x,
    NS_xls_xml_html,
    nullptr
};

}

const xmlns_id_t* NS_xls_xml_all = xls_xml_ns;

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
