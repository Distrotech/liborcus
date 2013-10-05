/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_ods.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/factory.hpp"

#include "orcus_filter_global.hpp"

#include <cstdlib>
#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace orcus;

int main(int argc, char** argv)
{
    boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
    boost::scoped_ptr<spreadsheet::import_factory> fact(new spreadsheet::import_factory(doc.get()));
    orcus_ods app(fact.get());

    if (parse_import_filter_args(app, *doc, argc, argv))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
