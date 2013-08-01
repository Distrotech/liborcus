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

#include "orcus/orcus_ods.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"
#include "orcus/spreadsheet/factory.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/shared_strings.hpp"
#include "orcus/spreadsheet/styles.hpp"

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

#include <mdds/flat_segment_tree.hpp>

using namespace orcus;
using namespace orcus::spreadsheet;
using namespace std;

typedef mdds::flat_segment_tree<size_t,bool> bool_segment_type;

namespace {

const char* dirs[] = {
    SRCDIR"/test/ods/raw-values-1/",
};

void test_ods_import_cell_values()
{
    size_t n = sizeof(dirs)/sizeof(dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = dirs[i];
        string path(dir);

        // Read the input.ods document.
        path.append("input.ods");
        spreadsheet::document doc;
        spreadsheet::import_factory factory(&doc);
        orcus_ods app(&factory);
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

void test_ods_import_column_widths_row_heights()
{
    const char* filepath = SRCDIR"/test/ods/column-width-row-height/input.ods";
    document doc;
    import_factory factory(&doc);
    orcus_ods app(&factory);
    app.read_file(filepath);

    assert(doc.sheet_size() > 0);
    spreadsheet::sheet* sh = doc.get_sheet(0);
    assert(sh);

    // Column widths are in twips.
    col_width_t cw = sh->get_col_width(1, NULL, NULL);
    assert(cw == 1440); // 1 in
    cw = sh->get_col_width(2, NULL, NULL);
    assert(cw == 2160); // 1.5 in
    cw = sh->get_col_width(3, NULL, NULL);
    assert(cw == 2592); // 1.8 in

    // Row heights are in twips too.
    row_height_t rh = sh->get_row_height(3, NULL, NULL);
    assert(rh == 720); // 0.5 in
    rh = sh->get_row_height(4, NULL, NULL);
    assert(rh == 1440); // 1 in
    rh = sh->get_row_height(5, NULL, NULL);
    assert(rh == 2160); // 1.5 in
    rh = sh->get_row_height(6, NULL, NULL);
    assert(rh == 2592); // 1.8 in
}

void test_ods_import_formatted_text()
{
    const char* filepath = SRCDIR"/test/ods/formatted-text/bold-and-italic.ods";
    document doc;
    import_factory factory(&doc);
    orcus_ods app(&factory);
    app.read_file(filepath);

    assert(doc.sheet_size() > 0);
    spreadsheet::sheet* sh = doc.get_sheet(0);
    assert(sh);

    const import_shared_strings* ss = doc.get_shared_strings();
    assert(ss);

    const import_styles* styles = doc.get_styles();
    assert(styles);

    // A1 is unformatted
    size_t str_id = sh->get_string_identifier(0,0);
    const string* str = ss->get_string(str_id);
    assert(str && *str == "Normal Text");
    size_t xfid = sh->get_cell_format(0,0);
    assert(xfid == 0); // ID of 0 represents default format.
    const format_runs_t* fmt = ss->get_format_runs(str_id);
    assert(!fmt); // The string should be unformatted.

    // A2 is all bold via cell format.
    str_id = sh->get_string_identifier(1,0);
    str = ss->get_string(str_id);
    assert(str && *str == "Bold Text");
    xfid = sh->get_cell_format(1,0);
    const cell_format* xf = styles->get_cell_format(xfid);
    assert(xf);
    const font* font_data = styles->get_font(xf->font);
    assert(font_data && font_data->bold && !font_data->italic);
    fmt = ss->get_format_runs(str_id);
    assert(!fmt); // This string should be unformatted.

    // A3 is all italic.
    str_id = sh->get_string_identifier(2,0);
    str = ss->get_string(str_id);
    assert(str && *str == "Italic Text");
    xfid = sh->get_cell_format(2,0);
    xf = styles->get_cell_format(xfid);
    assert(xf);
    font_data = styles->get_font(xf->font);
    assert(font_data && !font_data->bold && font_data->italic);
    fmt = ss->get_format_runs(str_id);
    assert(!fmt); // This string should be unformatted.

    // A4 is all bolid and italic.
    str_id = sh->get_string_identifier(3,0);
    str = ss->get_string(str_id);
    assert(str && *str == "Bold and Italic Text");
    xfid = sh->get_cell_format(3,0);
    xf = styles->get_cell_format(xfid);
    assert(xf);
    font_data = styles->get_font(xf->font);
    assert(font_data && font_data->bold && font_data->italic);
    fmt = ss->get_format_runs(str_id);
    assert(!fmt); // This string should be unformatted.

    // A5 has mixed format runs.
    str_id = sh->get_string_identifier(4,0);
    str = ss->get_string(str_id);
    assert(str && *str == "Bold and Italic mixed");
    xfid = sh->get_cell_format(4,0);
    xf = styles->get_cell_format(xfid);
    assert(xf);
    font_data = styles->get_font(xf->font);
    fmt = ss->get_format_runs(str_id);
    assert(fmt); // This string should be formatted.

    {
        // Check the bold format segment.
        bool_segment_type bold_runs(0, str->size(), font_data->bold);
        for (size_t i = 0, n = fmt->size(); i < n; ++i)
        {
            format_run run = fmt->at(i);
            bold_runs.insert_back(run.pos, run.pos+run.size, run.bold);
        }

        bold_runs.build_tree();
        bool is_bold = false;
        size_t start_pos, end_pos;

        // The first four letters 'Bold' should be bold.
        bool good = bold_runs.search_tree(0, is_bold, &start_pos, &end_pos).second;
        assert(good);
        assert(is_bold);
        assert(start_pos == 0);
        assert(end_pos == 4);

        // The rest should be non-bold.
        good = bold_runs.search_tree(4, is_bold, &start_pos, &end_pos).second;
        assert(good);
        assert(!is_bold);
        assert(start_pos == 4);
        assert(end_pos == str->size());
    }

    {
        // Check the italic format segment.
        bool_segment_type italic_runs(0, str->size(), font_data->italic);
        for (size_t i = 0, n = fmt->size(); i < n; ++i)
        {
            format_run run = fmt->at(i);
            italic_runs.insert_back(run.pos, run.pos+run.size, run.italic);
        }

        italic_runs.build_tree();
        bool it_italic = false;
        size_t start_pos, end_pos;

        // The first 9 letters 'Bold and ' should not be italic.
        bool good = italic_runs.search_tree(0, it_italic, &start_pos, &end_pos).second;
        assert(good);
        assert(!it_italic);
        assert(start_pos == 0);
        assert(end_pos == 9);

        // The next 6 letters 'Italic' should be italic.
        good = italic_runs.search_tree(9, it_italic, &start_pos, &end_pos).second;
        assert(good);
        assert(it_italic);
        assert(start_pos == 9);
        assert(end_pos == 15);

        // The rest should be non-italic.
        good = italic_runs.search_tree(15, it_italic, &start_pos, &end_pos).second;
        assert(good);
        assert(!it_italic);
        assert(start_pos == 15);
        assert(end_pos == str->size());
    }
}

}

int main()
{
    test_ods_import_cell_values();
    test_ods_import_column_widths_row_heights();
    test_ods_import_formatted_text();
    return EXIT_SUCCESS;
}
