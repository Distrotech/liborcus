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

#include "orcus/sax_ns_parser.hpp"
#include "orcus/global.hpp"
#include "orcus/dom_tree.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/stream.hpp"

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

#include <boost/scoped_ptr.hpp>

using namespace orcus;
using namespace std;

class sax_handler
{
    dom_tree m_tree;

public:
    sax_handler(xmlns_context& cxt) : m_tree(cxt) {}

    void cdata(const pstring&) {}

    void start_declaration(const pstring& name)
    {
        m_tree.start_declaration(name);
    }

    void end_declaration(const pstring& name)
    {
        m_tree.end_declaration(name);
    }

    void start_element(const sax_ns_parser_element& elem)
    {
        m_tree.start_element(elem.ns, elem.name);
    }

    void end_element(const sax_ns_parser_element& elem)
    {
        m_tree.end_element(elem.ns, elem.name);
    }

    void characters(const pstring& val)
    {
        m_tree.set_characters(val);
    }

    void attribute(const pstring& name, const pstring& val)
    {
        m_tree.set_attribute(XMLNS_UNKNOWN_ID, name, val);
    }

    void attribute(const sax_ns_parser_attribute& attr)
    {
        m_tree.set_attribute(attr.ns, attr.name, attr.value);
    }

    const dom_tree& get_dom() const
    {
        return m_tree;
    }
};

const char* sax_parser_test_dirs[] = {
    SRCDIR"/test/xml/simple/",
    SRCDIR"/test/xml/encoded-char/",
    SRCDIR"/test/xml/default-ns/",
    SRCDIR"/test/xml/ns-alias-1/",
    SRCDIR"/test/xml/bom/",
    SRCDIR"/test/xml/custom-decl-1/"
};

const char* sax_parser_parse_only_test_dirs[] = {
    SRCDIR"/test/xml/parse-only/rss/"
};

sax_handler* parse_file(xmlns_context& cxt, const char* filepath)
{
    string strm;
    cout << "testing " << filepath << endl;
    load_file_content(filepath, strm);
    assert(!strm.empty());

    orcus::unique_ptr<sax_handler> hdl(new sax_handler(cxt));
    sax_ns_parser<sax_handler> parser(strm.c_str(), strm.size(), cxt, *hdl);
    parser.parse();

    // Every valid XML file must have <?xml....?> in it.
    assert(hdl->get_dom().get_declaration_attributes("xml"));

    return hdl.release();
}

void test_xml_sax_parser()
{
    size_t n = sizeof(sax_parser_test_dirs)/sizeof(sax_parser_test_dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = sax_parser_test_dirs[i];
        string dir_path(dir);
        string file = dir_path;
        file.append("input.xml");

        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();
        boost::scoped_ptr<sax_handler> hdl(parse_file(cxt, file.c_str()));

        // Get the compact form of the content.
        ostringstream os;
        hdl->get_dom().dump_compact(os);
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
    size_t n = sizeof(sax_parser_parse_only_test_dirs)/sizeof(sax_parser_parse_only_test_dirs[0]);
    for (size_t i = 0; i < n; ++i)
    {
        const char* dir = sax_parser_parse_only_test_dirs[i];
        string dir_path(dir);
        string file = dir_path;
        file.append("input.xml");

        xmlns_repository repo;
        xmlns_context cxt = repo.create_context();
        boost::scoped_ptr<sax_handler> hdl(parse_file(cxt, file.c_str()));
    }
}

void test_xml_declarations()
{
    const char* file_path = SRCDIR"/test/xml/custom-decl-1/input.xml";
    xmlns_repository repo;
    xmlns_context cxt = repo.create_context();
    boost::scoped_ptr<sax_handler> hdl(parse_file(cxt, file_path));

    const dom_tree& dom = hdl->get_dom();

    // Make sure we parse the custom declaration correctly.
    const dom_tree::attrs_type* p = dom.get_declaration_attributes("mso-application");
    assert(p);
    assert(p->size() == 1);
    const dom_tree::attr& at = (*p)[0];
    assert(at.name.ns == XMLNS_UNKNOWN_ID && at.name.name == "progid" && at.value == "Excel.Sheet");
}

int main()
{
    test_xml_sax_parser();
    test_xml_sax_parser_read_only();
    test_xml_declarations();

    return EXIT_SUCCESS;
}
