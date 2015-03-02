/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_parser.hpp"
#include "orcus/css_document_tree.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace orcus;
using namespace std;

namespace {

class parser_handler
{
    css_document_tree& m_doc;
    std::vector<css_selector_t> m_cur_selector_group;
    css_properties_t m_cur_properties;
    pstring m_cur_prop_name;
    css_selector_t m_cur_selector;  /// current selector
    bool m_in_prop:1;
public:
    parser_handler(css_document_tree& doc) : m_doc(doc), m_in_prop(false) {}

    void at_rule_name(const char* p, size_t n)
    {
        cout << "@" << string(p, n).c_str();
    }

    void simple_selector(const char* p_elem, size_t n_elem, const char* p_class, size_t n_class)
    {
        css_simple_selector_t ss;
        ss.name = pstring(p_elem, n_elem);
        pstring class_name(p_class, n_class);
        if (!class_name.empty())
            ss.classes.insert(class_name);

        cout << "(elem='" << string(p_elem, n_elem).c_str() << "'; class='" << string(p_class, n_class).c_str() << "') ";

        if (m_cur_selector.first.empty())
            m_cur_selector.first = ss;
        else
        {
            // TODO : this is currently not being handled correctly.
            css_chained_simple_selector_t css;
            css.combinator = css_combinator_descendant;
            css.simple_selector = ss;
            m_cur_selector.chained.push_back(css);
        }
    }

    void end_selector()
    {
        cout << "|";
        m_cur_selector_group.push_back(m_cur_selector);
        m_cur_selector.clear();
    }

    void property_name(const char* p, size_t n)
    {
        m_cur_prop_name = pstring(p, n);
        cout << string(p, n).c_str() << ":";
    }

    void value(const char* p, size_t n)
    {
        pstring pv(p, n);
        m_cur_properties.insert(
            css_properties_t::value_type(m_cur_prop_name, pv));
        m_cur_prop_name.clear();
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

        // Push the property set and selector group to the document tree.

        std::vector<css_selector_t>::iterator it = m_cur_selector_group.begin(), ite = m_cur_selector_group.end();
        for (; it != ite; ++it)
            m_doc.insert_properties(*it, m_cur_properties);

        m_cur_selector_group.clear();
        m_cur_properties.clear();
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
    css_document_tree m_doc;
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

    parser_handler handler(m_doc);
    css_parser<parser_handler> parser(&strm[0], strm.size(), handler);
    parser.parse();
}

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    orcus_css parser;
    parser.read_file(argv[1]);

    return EXIT_SUCCESS;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
