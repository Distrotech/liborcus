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

#include <ixion/address.hpp>

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

void test_xlsx_table()
{
    string path(SRCDIR"/test/xlsx/table/table-1.xlsx");
    document doc;
    import_factory factory(doc);
    orcus_xlsx app(&factory);
    app.read_file(path.c_str());

    pstring name("Table1");
    const table_t* p = doc.get_table(name);
    assert(p);
    assert(p->identifier == 1);
    assert(p->name == name);
    assert(p->display_name == name);
    assert(p->totals_row_count == 1);

    // Table range is C3:D9.
    ixion::abs_range_t range;
    range.first.column = 2;
    range.first.row = 2;
    range.first.sheet = 0;
    range.last.column = 3;
    range.last.row = 8;
    range.last.sheet = 0;
    assert(p->range == range);

    // Table1 has 2 table columns.
    assert(p->columns.size() == 2);

    const table_column_t* tcol = &p->columns[0];
    assert(tcol);
    assert(tcol->identifier == 1);
    assert(tcol->name == "Category");
    assert(tcol->totals_row_label == "Total");
    assert(tcol->totals_row_function == totals_row_function_none);

    tcol = &p->columns[1];
    assert(tcol);
    assert(tcol->identifier == 2);
    assert(tcol->name == "Value");
    assert(tcol->totals_row_label.empty());
    assert(tcol->totals_row_function == totals_row_function_sum);

    const auto_filter_t& filter = p->filter;

    // Auto filter range is C3:D8.
    range.last.row = 7;
    assert(filter.range == range);

    assert(filter.columns.size() == 1);
    const auto_filter_column_t& afc = filter.columns.begin()->second;
    assert(afc.match_values.size() == 4);
    assert(afc.match_values.count("A") > 0);
    assert(afc.match_values.count("C") > 0);
    assert(afc.match_values.count("D") > 0);
    assert(afc.match_values.count("E") > 0);
}

}

int main()
{
    test_xlsx_import();
    test_xlsx_table_autofilter();
    test_xlsx_table();
    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
