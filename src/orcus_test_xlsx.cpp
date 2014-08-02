/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/orcus_xlsx.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"
#include "orcus/spreadsheet/factory.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/auto_filter.hpp"

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

using namespace orcus;
using namespace orcus::spreadsheet;
using namespace std;

namespace {

const char* dirs[] = {
    SRCDIR"/test/xlsx/raw-values-1/",
    SRCDIR"/test/xlsx/empty-shared-strings/",
};

/**
 * Semi-automated import test that goes through all specified directories,
 * and in each directory, reads the input.xlsx file, dumps its output and
 * checks it against the check.txt content.
 */
void test_xlsx_import()
{
    size_t n = sizeof(dirs)/sizeof(dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = dirs[i];
        string path(dir);

        // Read the input.xlsx document.
        path.append("input.xlsx");
        spreadsheet::document doc;
        spreadsheet::import_factory factory(doc);
        orcus_xlsx app(&factory);
        app.read_file(path.c_str());

        // Dump the content of the model.
        ostringstream os;
        doc.dump_check(os);
        string check = os.str();

        // Check that against known control.
        path = dir;
        path.append("check.txt");
        string control;
        load_file_content(path.c_str(), control);

        assert(!check.empty());
        assert(!control.empty());

        pstring s1(&check[0], check.size()), s2(&control[0], control.size());
        assert(s1.trim() == s2.trim());
    }
}

void test_xlsx_table_autofilter()
{
    string path(SRCDIR"/test/xlsx/table/autofilter.xlsx");
    document doc;
    import_factory factory(doc);
    orcus_xlsx app(&factory);
    app.read_file(path.c_str());

    const sheet* sh = doc.get_sheet(0);
    assert(sh);
    const auto_filter_t* af = sh->get_auto_filter_data();
    assert(af);

    // Autofilter is over B2:C11.
    assert(af->range.first.column == 1);
    assert(af->range.first.row == 1);
    assert(af->range.last.column == 2);
    assert(af->range.last.row == 10);

    // Check the match values of the 1st column filter criterion.
    auto_filter_t::columns_type::const_iterator it = af->columns.find(0);
    assert(it != af->columns.end());

    const auto_filter_column_t* afc = &it->second;
    assert(afc->match_values.count("A") > 0);
    assert(afc->match_values.count("C") > 0);

    // And the 2nd column.
    it = af->columns.find(1);
    assert(it != af->columns.end());
    afc = &it->second;
    assert(afc->match_values.count("1") > 0);
}

}

int main()
{
    test_xlsx_import();
    test_xlsx_table_autofilter();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
