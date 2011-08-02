/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#include "orcus/css_parser.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace orcus;
using namespace std;

namespace {

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
    os << file.rdbuf() << ' '; // extra char as the end position.
    file.close();
    strm = os.str();
}

class parser_handler
{
    bool m_in_prop:1;
public:
    parser_handler() : m_in_prop(false) {}

    void at_rule_name(const char* p, size_t n)
    {
        cout << "@" << string(p, n).c_str();
    }

    void selector_name(const char* p, size_t n)
    {
        cout << string(p, n).c_str();
    }

    void property_name(const char* p, size_t n)
    {
        cout << string(p, n).c_str() << ":";
    }

    void value(const char* p, size_t n)
    {
        cout << " '" << string(p, n).c_str() << "'";
    }

    void begin_parse()
    {
        cout << "========" << endl;
    }

    void end_parse()
    {
        cout << "========" << endl;
    }

    void begin_block()
    {
        cout << endl << "{" << endl;
        m_in_prop = true;
    }

    void end_block()
    {
        cout << "}" << endl;
        m_in_prop = false;
    }

    void begin_property()
    {
        if (m_in_prop)
            cout << "    ";
        cout << "* ";
    }

    void end_property()
    {
        cout << endl;
    }
};

}

class orcus_css
{
public:
    orcus_css();
    ~orcus_css();

    void read_file(const char* filepath);
private:
    void parse(const string& strm);
};

orcus_css::orcus_css() {}
orcus_css::~orcus_css() {}

void orcus_css::read_file(const char* filepath)
{
    cout << "reading " << filepath << endl;
    string strm;
    load_file_content(filepath, strm);
    parse(strm);
}

void orcus_css::parse(const string& strm)
{
    if (strm.empty())
        return;

    cout << "original: '" << strm << "'" << endl << endl;

    parser_handler handler;
    css_parser<parser_handler> parser(&strm[0], strm.size(), handler);
    parser.parse();
}

int main(int argc, char** argv)
{
    if (argc != 2)
        return EXIT_FAILURE;

    orcus_css app;
    app.read_file(argv[1]);
    pstring::intern::dispose();

    return EXIT_SUCCESS;
}
