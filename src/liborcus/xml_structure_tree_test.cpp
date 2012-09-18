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

#include "orcus/xml_structure_tree.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/global.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;
using namespace orcus;

const char* basic_files[] = {
    "../../test/xml-structure/basic-1",
    "../../test/xml-structure/basic-2",
    "../../test/xml-structure/basic-3"
};

void test_basic()
{
    size_t n = sizeof(basic_files)/sizeof(basic_files[0]);
    for (size_t i = 0; i < n; ++i)
    {
        string filepath(basic_files[i]);
        filepath.append(".xml");
        string strm;
        load_file_content(filepath.c_str(), strm);
        assert(!strm.empty());
        xmlns_repository xmlns_repo;
        xml_structure_tree tree(xmlns_repo);
        tree.parse(&strm[0], strm.size());
        ostringstream os;
        tree.dump_compact(os);
        string data_content = os.str();
        cout << "--" << endl;
        cout << data_content;

        // Check the dump content against known datum.
        filepath = basic_files[i];
        filepath.append(".check");
        string strm_check;
        load_file_content(filepath.c_str(), strm_check);
        assert(!strm_check.empty());

        // They should be identical, plus or minus leading/trailing whitespaces.
        pstring s1(&data_content[0], data_content.size());
        pstring s2(&strm_check[0], strm_check.size());
        assert(s1.trim() == s2.trim());
    }
}

void test_walker()
{
    {
        string filepath(basic_files[0]);
        filepath.append(".xml");
        string strm;
        load_file_content(filepath.c_str(), strm);
        assert(!strm.empty());
        xmlns_repository xmlns_repo;
        xml_structure_tree tree(xmlns_repo);
        tree.parse(&strm[0], strm.size());

        xml_structure_tree::walker wkr = tree.get_walker();
        const xml_structure_tree::element* root = wkr.root();
    }
}

int main()
{
    test_basic();
    test_walker();
    return EXIT_SUCCESS;
}
