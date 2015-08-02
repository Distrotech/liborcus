/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_document_tree.hpp"
#include "orcus/config.hpp"
#include "orcus/stream.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/dom_tree.hpp"

#include "orcus_filter_global.hpp"

#include <iostream>
#include <fstream>
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
        std::string strm = load_file_content(config->input_path.c_str());

        json_document_tree doc;
        doc.load(strm, *config);

        switch (config->output_format)
        {
            case json_config::output_format_type::xml:
            {
                ofstream fs(config->output_path.c_str());
                fs << doc.dump_xml();
            }
            break;
            case json_config::output_format_type::json:
            {
                ofstream fs(config->output_path.c_str());
                fs << doc.dump();
            }
            break;
            case json_config::output_format_type::check:
            {
                string xml_strm = doc.dump_xml();
                xmlns_repository repo;
                xmlns_context cxt = repo.create_context();
                dom_tree dom(cxt);
                dom.load(xml_strm);

                ofstream fs(config->output_path.c_str());
                dom.dump_compact(fs);
            }
            break;
            default:
                ;
        }
    }
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
