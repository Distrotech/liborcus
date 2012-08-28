/*************************************************************************
 *
 * Copyright (c) 2011-2012 Kohei Yoshida
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

#include "orcus/orcus_csv.hpp"

#include "orcus/csv_parser.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <cstring>
#include <iostream>

using namespace std;

namespace orcus {

namespace {

class csv_handler
{
public:
    csv_handler(spreadsheet::iface::import_factory& factory) :
        m_factory(factory), mp_sheet(NULL), m_row(0), m_col(0) {}

    void begin_parse()
    {
        const char* sheet_name = "data";
        mp_sheet = m_factory.append_sheet(sheet_name, strlen(sheet_name));
    }

    void end_parse() {}
    void begin_row() {}

    void end_row()
    {
        ++m_row;
        m_col = 0;
    }

    void cell(const char* p, size_t n)
    {
        mp_sheet->set_auto(m_row, m_col, p, n);
        ++m_col;
    }

private:
    spreadsheet::iface::import_factory& m_factory;
    spreadsheet::iface::import_sheet* mp_sheet;
    spreadsheet::row_t m_row;
    spreadsheet::col_t m_col;
};

}

orcus_csv::orcus_csv(spreadsheet::iface::import_factory* factory) : mp_factory(factory) {}

void orcus_csv::read_file(const char* filepath)
{
    cout << "reading " << filepath << endl;
    string strm;
    load_file_content(filepath, strm);
    parse(strm);
}

void orcus_csv::parse(const string& strm)
{
    if (strm.empty())
        return;

    csv_handler handler(*mp_factory);
    csv_parser_config config;
    config.delimiters.push_back(',');
    config.text_qualifier = '"';
    csv_parser<csv_handler> parser(&strm[0], strm.size(), handler, config);
    try
    {
        parser.parse();
    }
    catch (const csv_parse_error& e)
    {
        cout << "parse failed: " << e.what() << endl;
    }
}

}

