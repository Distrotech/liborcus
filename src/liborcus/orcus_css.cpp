/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_parser.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"

#include <iostream>
#include <string>

using namespace orcus;
using namespace std;

namespace {

class parser_handler
{
    bool m_in_prop:1;
public:
    parser_handler() : m_in_prop(false) {}

    void at_rule_name(const char* p, size_t n)
    {
        cout << "@" << string(p, n).c_str();
    }

    void selector_name(const char* p_elem, size_t n_elem, const char* p_class, size_t n_class)
    {
        cout << "(elem='" << string(p_elem, n_elem).c_str() << "'; class='" << string(p_class, n_class).c_str() << "') ";
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

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
