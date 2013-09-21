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

#include "orcus/orcus_gnumeric.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/factory.hpp"
#include "orcus/pstring.hpp"

#include <iostream>

#include <boost/scoped_ptr.hpp>

using namespace orcus;

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
    boost::scoped_ptr<spreadsheet::import_factory> fact(new spreadsheet::import_factory(doc.get()));
    orcus_gnumeric app(fact.get());
    try
    {
        app.read_file(argv[1]);
    }
    catch(const std::exception& e)
    {
        std::cout << "Caught Exception: " << e.what() << std::endl;
    }
    doc->dump_flat("./flat");
    doc->dump_html("./html");

    return EXIT_SUCCESS;
}
