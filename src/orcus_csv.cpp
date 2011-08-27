/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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
#include "orcus/model/factory.hpp"
#include "orcus/model/document.hpp"

#include <cstring>
#include <iostream>
#include <boost/scoped_ptr.hpp>

using namespace orcus;
using namespace std;

namespace {

class csv_handler
{
public:
    csv_handler(model::interface::factory& factory) :
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
    model::interface::factory& m_factory;
    model::interface::sheet* mp_sheet;
    model::row_t m_row;
    model::col_t m_col;
};

}

namespace orcus {

orcus_csv::orcus_csv(model::interface::factory* factory) : mp_factory(factory) {}

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
    csv_parser<csv_handler> parser(&strm[0], strm.size(), handler, config);
    parser.parse();
}

}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    ::boost::scoped_ptr<model::document> doc(new model::document);

    orcus_csv app(new model::factory(doc.get()));
    app.read_file(argv[1]);
    doc->dump();
    doc->dump_html("./obj");
    pstring::intern::dispose();

    return EXIT_SUCCESS;
}
