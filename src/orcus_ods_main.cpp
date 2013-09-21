/*************************************************************************
 *
 * Copyright (c) 2011-2012 Kohei Yoshida
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

#include "orcus/orcus_ods.hpp"
#include "orcus/pstring.hpp"
#include "orcus/spreadsheet/document.hpp"
#include "orcus/spreadsheet/factory.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <vector>

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

using namespace std;
using namespace orcus;

int main(int argc, char** argv)
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
        return EXIT_FAILURE;
    }

    if (vm.count("help"))
    {
        cout << help_program << endl << endl << desc;
        return EXIT_SUCCESS;
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
        return EXIT_FAILURE;
    }

    if (outdir.empty())
    {
        cerr << "No output directory." << endl;
        return EXIT_FAILURE;
    }

    if (outformat.empty())
    {
        cerr << "No output format specified.  Choose either 'flat' or 'html'." << endl;
        return EXIT_FAILURE;
    }

    if (fs::exists(outdir))
    {
        if (!fs::is_directory(outdir))
        {
            cerr << "A file named '" << outdir << "' already exists, and is not a directory." << endl;
            return EXIT_FAILURE;
        }
    }
    else
        fs::create_directory(outdir);

    boost::scoped_ptr<spreadsheet::document> doc(new spreadsheet::document);
    boost::scoped_ptr<spreadsheet::import_factory> fact(new spreadsheet::import_factory(doc.get()));
    orcus_ods app(fact.get());
    app.read_file(infile.c_str());

    if (outformat == "flat")
        doc->dump();
    else if (outformat == "html")
        doc->dump_html(outdir);

    return EXIT_SUCCESS;
}
