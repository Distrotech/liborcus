/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include <cstdlib>

#include "orcus/sax_parser.hpp"
#include "orcus/global.hpp"
#include "orcus/dom_tree.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace orcus;
using namespace std;

class sax_handler
{
    dom_tree m_tree;

public:
    void declaration()
    {
        m_tree.end_declaration();
    }

    void start_element(const pstring& ns, const pstring& name)
    {
        m_tree.start_element(ns, name);
    }

    void end_element(const pstring& ns, const pstring& name)
    {
        m_tree.end_element(ns, name);
    }

    void characters(const pstring& val)
    {
        m_tree.set_characters(val);
    }

    void attribute(const pstring& ns, const pstring& name, const pstring& val)
    {
        m_tree.set_attribute(ns, name, val);
    }

    void dump(ostream& os)
    {
        m_tree.dump_compact(os);
    }
};

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    string strm;
    load_file_content(argv[1], strm);
    if (strm.empty())
        return EXIT_FAILURE;

    sax_handler hdl;
    sax_parser<sax_handler> parser(strm.c_str(), strm.size(), hdl);
    parser.parse();
    ostringstream os;
    hdl.dump(os);
    cout << os.str();

    return EXIT_SUCCESS;
}
