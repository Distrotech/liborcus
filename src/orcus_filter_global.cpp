/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus_filter_global.hpp"
#include "orcus/pstring.hpp"
#include "orcus/config.hpp"
#include "orcus/interface.hpp"
#include "orcus/global.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace orcus;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace orcus {

namespace {

const char* help_program =
"The FILE must specify a path to an existing file.";

const char* help_output =
"Output directory path, or output file when --dump-check option is used.";

const char* help_output_format =
"Specify the format of output file.  Supported format types are:\n"
"  1) flat text format (flat)\n"
"  2) HTML format (html)\n"
"  3) JSON (json)\n"
"  4) no output (none)";

const char* help_dump_check =
"Dump the the content to stdout in a special format used for content verification in unit tests.";

const char* help_debug =
"Turn on a debug mode to generate run-time debug output.";

const char* help_json_output =
"Output file path.";

const char* help_json_output_format =
"Specify the format of output file.  Supported format types are:\n"
"  1) XML (xml)\n"
"  2) JSON (json)\n"
"  3) flat tree dump (check)\n"
"  4) no output (none)";

const char* err_no_input_file = "No input file.";

}

bool handle_dump_check(
    iface::import_filter& app, iface::document_dumper& doc, const string& infile, const string& outfile)
{
    if (outfile.empty())
    {
        // Dump to stdout when no output file is specified.
        app.read_file(infile);
        doc.dump_check(cout);
        return true;
    }

    if (fs::exists(outfile) && fs::is_directory(outfile))
    {
        cerr << "A directory named '" << outfile << "' already exists." << endl;
        return false;
    }

    ofstream file(outfile.c_str());
    app.read_file(infile);
    doc.dump_check(file);
    return true;
}

bool parse_import_filter_args(
    iface::import_filter& app, iface::document_dumper& doc, int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Print this help.")
        ("debug,d", help_debug)
        ("dump-check", help_dump_check)
        ("output,o", po::value<string>(), help_output)
        ("output-format,f", po::value<string>(), help_output_format);

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
        cout << e.what() << endl;
        cout << desc;
        return false;
    }

    if (vm.count("help"))
    {
        cout << "Usage: orcus-" << app.get_name() << " [options] FILE" << endl << endl;
        cout << help_program << endl << endl << desc;
        return true;
    }

    string infile, outdir, outformat;

    if (vm.count("input"))
        infile = vm["input"].as<string>();

    if (vm.count("output"))
        outdir = vm["output"].as<string>();

    if (vm.count("output-format"))
        outformat = vm["output-format"].as<string>();

    if (infile.empty())
    {
        cerr << err_no_input_file << endl;
        return false;
    }

    if (vm.count("dump-check"))
    {
        // 'outdir' is used as the output file path in this mode.
        return handle_dump_check(app, doc, infile, outdir);
    }

    config opt;
    opt.debug = vm.count("debug") > 0;
    app.set_config(opt);

    if (outformat.empty())
    {
        cerr << "No output format specified.  Choose either 'flat', 'html' or 'none'." << endl;
        return false;
    }

    if (outformat == "none")
    {
        // When "none" format is specified, just read the input file and exit.
        app.read_file(infile);
        return true;
    }

    if (outdir.empty())
    {
        cerr << "No output directory." << endl;
        return false;
    }

    if (fs::exists(outdir))
    {
        if (!fs::is_directory(outdir))
        {
            cerr << "A file named '" << outdir << "' already exists, and is not a directory." << endl;
            return false;
        }
    }
    else
        fs::create_directory(outdir);

    app.read_file(infile);

    if (outformat == "flat")
        doc.dump_flat(outdir);
    else if (outformat == "html")
        doc.dump_html(outdir);
    else if (outformat == "json")
        doc.dump_json(outdir);
    else
    {
        // Do nothing, but warning about unknown output format type.
        cerr << "Unknown output format type '" << outformat << "'. No output files have been generated." << endl;
    }

    return true;
}

void print_json_usage(std::ostream& os, const po::options_description& desc)
{
    os << "Usage: orcus-json [options] FILE" << endl << endl;
    os << help_program << endl << endl << desc;
}

std::unique_ptr<json_config> parse_json_args(int argc, char** argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Print this help.")
        ("resolve-refs", "Resolve JSON references to external files.")
        ("output,o", po::value<string>(), help_json_output)
        ("output-format,f", po::value<string>(), help_json_output_format);

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
        print_json_usage(cerr, desc);
        return nullptr;
    }

    if (vm.count("help"))
    {
        print_json_usage(cout, desc);
        return nullptr;
    }

    std::unique_ptr<json_config> config = orcus::make_unique<json_config>();

    if (vm.count("input"))
        config->input_path = vm["input"].as<string>();

    if (vm.count("output"))
        config->output_path = vm["output"].as<string>();

    if (vm.count("resolve-refs"))
        config->resolve_references = true;

    if (vm.count("output-format"))
    {
        std::string outformat = vm["output-format"].as<string>();
        if (outformat == "none")
            config->output_format = json_config::output_format_type::none;
        else if (outformat == "xml")
            config->output_format = json_config::output_format_type::xml;
        else if (outformat == "json")
            config->output_format = orcus::json_config::output_format_type::json;
        else if (outformat == "check")
            config->output_format = orcus::json_config::output_format_type::check;
        else
        {
            cerr << "Unknown output format type '" << outformat << "'." << endl;
            return nullptr;
        }
    }
    else
    {
        cerr << "Output format is not specified." << endl;
        print_json_usage(cerr, desc);
        return nullptr;
    }

    if (config->input_path.empty())
    {
        cerr << err_no_input_file << endl;
        print_json_usage(cerr, desc);
        return nullptr;
    }

    if (!fs::exists(config->input_path))
    {
        cerr << "Input file does not exist: " << config->input_path << endl;
        return nullptr;
    }

    if (config->output_format != json_config::output_format_type::none)
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

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
