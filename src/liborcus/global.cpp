/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#include "orcus/global.hpp"
#include "orcus/tokens.hpp"
#include "orcus/exception.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

struct attr_printer : unary_function<void, xml_token_attr_t>
{
public:
    attr_printer(const tokens& tokens) :
        m_tokens(tokens) {}

    void operator()(const xml_token_attr_t& attr) const
    {
        cout << "  ";
        if (attr.ns != XMLNS_UNKNOWN_TOKEN)
            cout << m_tokens.get_nstoken_name(attr.ns) << ":";

        cout << m_tokens.get_token_name(attr.name) << " = \"" << attr.value << "\"" << endl;
    }

private:
    const tokens& m_tokens;
};

}

void print_element(const tokens& tokens, xmlns_token_t ns, xml_token_t name)
{
    if (ns != XMLNS_UNKNOWN_TOKEN)
        cout << tokens.get_nstoken_name(ns) << ":";
    cout << tokens.get_token_name(name) << endl;
}

void print_attrs(const tokens& tokens, const xml_attrs_t& attrs)
{
    for_each(attrs.begin(), attrs.end(), attr_printer(tokens));
}

void load_file_content(const char* filepath, string& strm)
{
    ifstream file(filepath);
    if (!file)
    {
        // failed to open the specified file.
        ostringstream os;
        os << "failed to load " << filepath;
        throw general_error(os.str());
    }

    ostringstream os;
    os << file.rdbuf();
    file.close();
    strm = os.str();
}

}
