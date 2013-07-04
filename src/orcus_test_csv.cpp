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

#include "orcus/orcus_csv.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"
#include "orcus/spreadsheet/factory.hpp"
#include "orcus/spreadsheet/document.hpp"

#include <boost/scoped_ptr.hpp>

#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>

using namespace orcus;
using namespace std;

namespace {

const char* dirs[] = {
    SRCDIR"/test/csv/simple-numbers/",
    SRCDIR"/test/csv/normal-quotes/",
    SRCDIR"/test/csv/double-quotes/",
    SRCDIR"/test/csv/quoted-with-delim/",
};

void test_csv_import()
{
    size_t n = sizeof(dirs)/sizeof(dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = dirs[i];
        string path(dir);

        // Read the input.csv document.
        path.append("input.csv");
        boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
        orcus_csv app(new spreadsheet::import_factory(doc.get()));
        app.read_file(path.c_str());

        // Dump the content of the model.
        ostringstream os;
        doc->dump_check(os);
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

}

int main()
{
    test_csv_import();
    return EXIT_SUCCESS;
}
