/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus_filter_global.hpp"
#include "orcus/pstring.hpp"
#include "orcus/interface.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace orcus;

namespace orcus {

namespace {

const char* help_program =
"The FILE must specify a path to an existing file.";

const char* help_output =
"Output directory path.";

const char* help_output_format =
"Specify the format of output file.  Supported format types are: "
"1) flat text format (flat), 2) HTML format (html), or 3) no output (none).";

}

bool parse_import_filter_args(
    iface::import_filter& app, iface::document_dumper& doc, int argc, char** argv)
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Print this help.")
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
        cerr << "No input file." << endl;
        return false;
    }

    if (outdir.empty())
    {
        cerr << "No output directory." << endl;
        return false;
    }

    if (outformat.empty())
    {
        cerr << "No output format specified.  Choose either 'flat', 'html' or 'none'." << endl;
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
    else if (outformat == "none")
    {
        // Do nothing.
    }
    else
    {
        // Do nothing, but warning about unknown output format type.
        cerr << "Unknown output format type '" << outformat << "'. No output files have been generated." << endl;
    }

    return true;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
