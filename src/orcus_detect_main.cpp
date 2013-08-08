/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#include "orcus/format_detection.hpp"
#include "orcus/exception.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace orcus;
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    const char* filepath = argv[1];
    string strm;
    try
    {
        load_file_content(filepath, strm);
    }
    catch (const general_error& e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (strm.empty())
    {
        cerr << "file is empty" << endl;
        return EXIT_FAILURE;
    }

    format_t detected_type = detect(reinterpret_cast<const unsigned char*>(&strm[0]), strm.size());

    cout << "type: ";
    switch (detected_type)
    {
        case format_csv:
            cout << "plain text format";
        break;
        case format_gnumeric:
            cout << "Gnumeric";
        break;
        case format_ods:
            cout << "OpenDocument Spreadsheet";
        break;
        case format_xls_xml:
            cout << "Microsoft Excel XML";
        break;
        case format_xlsx:
            cout << "Microsoft Office Open XML Excel 2007+";
        break;
        case format_unknown:
        default:
            cout << "unknown";
    }
    cout << endl;

    return EXIT_SUCCESS;
}
