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

    void start_element(const sax_parser_element& elem)
    {
        m_tree.start_element(elem.ns, elem.name);
    }

    void end_element(const sax_parser_element& elem)
    {
        m_tree.end_element(elem.ns, elem.name);
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

const char* dirs[] = {
    "../test/xml/simple/"
};

void test_xml_sax_parser()
{
    size_t n = sizeof(dirs)/sizeof(dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = dirs[i];
        string dir_path(dir);
        string file = dir_path;
        file.append("/input.xml");
        string strm;
        cout << "testing " << file << endl;
        load_file_content(file.c_str(), strm);
        assert(!strm.empty());

        sax_handler hdl;
        sax_parser<sax_handler> parser(strm.c_str(), strm.size(), hdl);
        parser.parse();

        // Get the compact form of the content.
        ostringstream os;
        hdl.dump(os);
        strm = os.str(); // re-use this.

        // Load the check form.
        string check;
        file = dir_path;
        file.append("/check.txt");
        load_file_content(file.c_str(), check);
        pstring psource(strm.c_str(), strm.size());
        pstring pcheck(check.c_str(), check.size());

        // They must be equal, minus preceding or trailing spaces (if any).
        assert(psource.trim() == pcheck.trim());
    }
}

int main()
{
    test_xml_sax_parser();

    return EXIT_SUCCESS;
}
