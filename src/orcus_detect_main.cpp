/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
