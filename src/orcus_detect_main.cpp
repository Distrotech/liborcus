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

int main(int argc, char** argv) try
{
    if (argc != 2)
        return EXIT_FAILURE;

    const char* filepath = argv[1];
    string strm;
    load_file_content(filepath, strm);

    if (strm.empty())
    {
        cerr << "file is empty" << endl;
        return EXIT_FAILURE;
    }

    format_t detected_type = detect(reinterpret_cast<const unsigned char*>(&strm[0]), strm.size());

    cout << "type: ";
    switch (detected_type)
    {
        case format_t::csv:
            cout << "plain text format";
        break;
        case format_t::gnumeric:
            cout << "Gnumeric";
        break;
        case format_t::ods:
            cout << "OpenDocument Spreadsheet";
        break;
        case format_t::xls_xml:
            cout << "Microsoft Excel XML";
        break;
        case format_t::xlsx:
            cout << "Microsoft Office Open XML Excel 2007+";
        break;
        case format_t::unknown:
        default:
            cout << "unknown";
    }
    cout << endl;

    return EXIT_SUCCESS;
}
catch (const general_error& e)
{
    cerr << e.what() << endl;
    return EXIT_FAILURE;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
