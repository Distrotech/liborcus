/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/sax_ns_parser.hpp"
#include "orcus/global.hpp"
#include "orcus/dom_tree.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

using namespace orcus;
using namespace std;

class sax_handler_encoded_attrs
{
    std::vector<sax::parser_attribute> m_attrs;

public:
    void doctype(const sax::doctype_declaration&) {}

    void start_declaration(const pstring&) {}

    void end_declaration(const pstring&)
    {
        m_attrs.clear();
    }

    void start_element(const sax::parser_element&) {}

    void end_element(const sax::parser_element&) {}

    void characters(const pstring&, bool) {}

    void attribute(const sax::parser_attribute& attr)
    {
        m_attrs.push_back(attr);
    }

    bool check(const vector<string>& expected) const
    {
        if (m_attrs.size() != expected.size())
        {
            cerr << "unexpected attribute count." << endl;
            return false;
        }

        for (size_t i = 0, n = m_attrs.size(); i < n; ++i)
        {
            if (m_attrs[i].value != expected[i].c_str())
            {
                cerr << "expected attribute value: " << expected[i] << "  actual attribute value: " << m_attrs[i].value << endl;
                return false;
            }
        }

        return true;
    }
};

const char* sax_parser_test_dirs[] = {
    SRCDIR"/test/xml/simple/",
    SRCDIR"/test/xml/encoded-char/",
    SRCDIR"/test/xml/default-ns/",
    SRCDIR"/test/xml/ns-alias-1/",
    SRCDIR"/test/xml/bom/",
    SRCDIR"/test/xml/custom-decl-1/",
    SRCDIR"/test/xml/cdata-1/"
};

const char* sax_parser_parse_only_test_dirs[] = {
    SRCDIR"/test/xml/parse-only/rss/"
};

void parse_file(dom_tree& tree, const char* filepath, string& strm)
{
    cout << "testing " << filepath << endl;
    load_file_content(filepath, strm);
    assert(!strm.empty());

    tree.load(strm);
}

void test_xml_sax_parser()
{
    string strm;
    size_t n = sizeof(sax_parser_test_dirs)/sizeof(sax_parser_test_dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = sax_parser_test_dirs[i];
        string dir_path(dir);
        string file = dir_path;
        file.append("input.xml");

        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();
        dom_tree tree(cxt);
        parse_file(tree, file.c_str(), strm);

        // Get the compact form of the content.
        ostringstream os;
        tree.dump_compact(os);
        string content = os.str();

        // Load the check form.
        string check;
        file = dir_path;
        file.append("check.txt");
        load_file_content(file.c_str(), check);
        pstring psource(content.c_str(), content.size());
        pstring pcheck(check.c_str(), check.size());

        // They must be equal, minus preceding or trailing spaces (if any).
        assert(psource.trim() == pcheck.trim());
    }
}

void test_xml_sax_parser_read_only()
{
    string strm;
    size_t n = sizeof(sax_parser_parse_only_test_dirs)/sizeof(sax_parser_parse_only_test_dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = sax_parser_parse_only_test_dirs[i];
        string dir_path(dir);
        string file = dir_path;
        file.append("input.xml");

        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();
        dom_tree tree(cxt);
        parse_file(tree, file.c_str(), strm);
    }
}

void test_xml_declarations()
{
    string strm;
    const char* file_path = SRCDIR"/test/xml/custom-decl-1/input.xml";
    xmlns_repository repo;
    xmlns_context cxt = repo.create_context();
    dom_tree dom(cxt);
    parse_file(dom, file_path, strm);

    // Make sure we parse the custom declaration correctly.
    const dom_tree::attrs_type* p = dom.get_declaration_attributes("mso-application");
    assert(p);
    assert(p->size() == 1);
    const dom_tree::attr& at = (*p)[0];
    assert(at.name.ns == XMLNS_UNKNOWN_ID && at.name.name == "progid" && at.value == "Excel.Sheet");
}

void test_xml_dtd()
{
    struct {
        const char* file_path;
        sax::doctype_declaration::keyword_type keyword;
        const char* root_element;
        const char* fpi;
        const char* uri;
    } tests[] = {
        { SRCDIR"/test/xml/doctype/html.xml", sax::doctype_declaration::keyword_type::dtd_public,
          "html", "-//W3C//DTD XHTML 1.0 Transitional//EN", "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" }
    };

    string strm;
    xmlns_repository repo;
    size_t n = sizeof(tests)/sizeof(tests[0]);

    for (size_t i = 0; i < n; ++i)
    {
        const char* file_path = tests[i].file_path;
        string strm;
        xmlns_context cxt = repo.create_context();
        dom_tree dom(cxt);
        parse_file(dom, file_path, strm);
        const sax::doctype_declaration* dtd = dom.get_doctype();
        assert(dtd);
        assert(dtd->keyword == tests[i].keyword);
        assert(dtd->root_element == tests[i].root_element);
        assert(dtd->fpi == tests[i].fpi);
        if (tests[i].uri)
        {
            assert(dtd->uri == tests[i].uri);
        }
    }
}

void test_xml_encoded_attrs()
{
    const char* filepath = SRCDIR"/test/xml/encoded-attrs/test1.xml";

    string strm;
    cout << "testing " << filepath << endl;
    load_file_content(filepath, strm);
    assert(!strm.empty());

    sax_handler_encoded_attrs hdl;
    sax_parser<sax_handler_encoded_attrs> parser(&strm[0], strm.size(), hdl);
    parser.parse();

    vector<string> expected;
    expected.push_back("1 & 2");
    expected.push_back("3 & 4");
    expected.push_back("5 & 6");
    assert(hdl.check(expected));
}

int main()
{
    test_xml_sax_parser();
    test_xml_sax_parser_read_only();
    test_xml_declarations();
    test_xml_dtd();
    test_xml_encoded_attrs();

    return EXIT_SUCCESS;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
