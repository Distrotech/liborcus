/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_document_tree.hpp"
#include "orcus/config.hpp"
#include "orcus/stream.hpp"

#include "orcus_filter_global.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace orcus;

int main(int argc, char** argv)
{
    std::unique_ptr<json_config> config = parse_json_args(argc, argv);
    if (!config)
        return EXIT_FAILURE;

    try
    {
        std::string strm;
        load_file_content(config->input_path.c_str(), strm);

        json_document_tree doc;
        doc.load(strm);

        // TODO : continue....
    }
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
