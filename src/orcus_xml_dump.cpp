/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstdlib>

#include "orcus/sax_ns_parser.hpp"
#include "orcus/global.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/stream.hpp"

#include "dom_tree_sax_handler.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace orcus;
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    string strm;
    try
    {
        strm = load_file_content(argv[1]);
    }
    catch (const std::exception& e)
    {
        cerr << "exception caught while loading file: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (strm.empty())
        return EXIT_FAILURE;

    try
    {
        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();
        dom_tree_sax_handler hdl(cxt);
        sax_ns_parser<dom_tree_sax_handler> parser(strm.c_str(), strm.size(), cxt, hdl);
        parser.parse();
        ostringstream os;
        hdl.dump_compact(os);
        cout << os.str();
    }
    catch (const std::exception& e)
    {
        cerr << "exception caught while parsing file: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
