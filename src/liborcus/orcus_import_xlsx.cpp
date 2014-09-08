/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_import_xlsx.hpp"

#include "orcus/xml_namespace.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/config.hpp"

#include "xlsx_types.hpp"
#include "xlsx_handler.hpp"
#include "xlsx_context.hpp"
#include "ooxml_tokens.hpp"

#include "xml_stream_parser.hpp"
#include "ooxml_namespace_types.hpp"
#include "xlsx_session_data.hpp"
#include "ooxml_global.hpp"

#include <boost/scoped_ptr.hpp>

namespace orcus {

void import_xlsx::read_table(const char* p, size_t n, spreadsheet::iface::import_table* table)
{
    if(!table)
        return;

    if (!p || !n)
        return;

    session_context cxt;
    boost::scoped_ptr<xlsx_table_xml_handler> handler(
        new xlsx_table_xml_handler(cxt, ooxml_tokens, *table));

    xmlns_repository ns_repo;
    ns_repo.add_predefined_values(NS_ooxml_all);
    ns_repo.add_predefined_values(NS_opc_all);
    ns_repo.add_predefined_values(NS_misc_all);

    orcus::config config;
    xml_stream_parser parser(
        config, ns_repo, ooxml_tokens,
        p, n);
    parser.set_handler(handler.get());
    parser.parse();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
