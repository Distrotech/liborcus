/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/config.hpp"
#include "orcus/stream.hpp"
#include "orcus/global.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace orcus;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

const char* help_program = "The FILE must specify a path to an existing file.";
const char* err_no_input_file = "No input file.";
const char* help_yaml_output = "Output file path.";
const char* help_yaml_output_format = "Specify the format of output file.";

void print_yaml_usage(std::ostream& os, const po::options_description& desc)
{
    os << "Usage: orcus-yaml [options] FILE" << endl << endl;
    os << help_program << endl << endl << desc;
}

std::unique_ptr<yaml_config> parse_yaml_args(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Print this help.")
        ("output,o", po::value<string>(), help_yaml_output)
        ("output-format,f", po::value<string>(), help_yaml_output_format);

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("input", po::value<string>(), "input file");

    po::options_description cmd_opt;
    cmd_opt.add(desc).add(hidden);

    po::positional_options_description po_desc;
    po_desc.add("input", -1);

    po::variables_map vm;
    try
    {
        po::store(
            po::command_line_parser(argc, argv).options(cmd_opt).positional(po_desc).run(), vm);
        po::notify(vm);
    }
    catch (const exception& e)
    {
        // Unknown options.
        cerr << e.what() << endl;
        print_yaml_usage(cerr, desc);
        return nullptr;
    }

    if (vm.count("help"))
    {
        print_yaml_usage(cout, desc);
        return nullptr;
    }

    std::unique_ptr<yaml_config> config = make_unique<yaml_config>();

    if (vm.count("input"))
        config->input_path = vm["input"].as<string>();

    if (vm.count("output"))
        config->output_path = vm["output"].as<string>();

    if (vm.count("output-format"))
    {
        std::string outformat = vm["output-format"].as<string>();
        if (outformat == "none")
            config->output_format = yaml_config::output_format_type::none;
        else if (outformat == "yaml")
            config->output_format = yaml_config::output_format_type::yaml;
        else
        {
            cerr << "Unknown output format type '" << outformat << "'." << endl;
            return nullptr;
        }
    }
    else
    {
        cerr << "Output format is not specified." << endl;
        print_yaml_usage(cerr, desc);
        return nullptr;
    }

    if (config->input_path.empty())
    {
        cerr << err_no_input_file << endl;
        print_yaml_usage(cerr, desc);
        return nullptr;
    }

    if (!fs::exists(config->input_path))
    {
        cerr << "Input file does not exist: " << config->input_path << endl;
        return nullptr;
    }

    if (config->output_format != yaml_config::output_format_type::none)
    {
        if (config->output_path.empty())
        {
            cerr << "Output file not given." << endl;
            return nullptr;
        }

        // Check to make sure the output path doesn't point to an existing
        // directory.
        if (fs::is_directory(config->output_path))
        {
            cerr << "Output file path points to an existing directory.  Aborting." << endl;
            return nullptr;
        }
    }

    return config;
}

int main(int argc, char** argv)
{
    std::unique_ptr<yaml_config> config = parse_yaml_args(argc, argv);
    if (!config)
        return EXIT_FAILURE;

    try
    {
        std::string strm = load_file_content(config->input_path.c_str());

        yaml_document_tree doc;
        doc.load(strm);

        switch (config->output_format)
        {
            case yaml_config::output_format_type::yaml:
            {
                ofstream fs(config->output_path.c_str());
                fs << doc.dump_yaml();
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
