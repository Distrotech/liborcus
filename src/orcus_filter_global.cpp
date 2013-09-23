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

#include "orcus_filter_global.hpp"
#include "orcus/orcus_ods.hpp"
#include "orcus/pstring.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <vector>

using namespace std;
using namespace orcus;

namespace orcus {

namespace {

const char* help_program =
"Usage: orcus-ods [options] FILE\n\n"
"The FILE must specify a path to an existing ODF spreadsheet file.";

const char* help_output =
"Output directory path.";

const char* help_output_format =
"Specify the format of output file.  Supported format types are: "
"1) flat text format (flat), or 2) HTML format (html).";

}

bool parse_args(iface::import_filter* app, iface::document_dumper* doc, int argc, char** argv)
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
        cerr << "No output format specified.  Choose either 'flat' or 'html'." << endl;
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

    app->read_file(infile);

    if (outformat == "flat")
        doc->dump_flat(outdir);
    else if (outformat == "html")
        doc->dump_html(outdir);

    return true;
}

}
