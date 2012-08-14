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

#include "orcus/orcus_xml.hpp"
#include "orcus/global.hpp"
#include "orcus/sax_parser.hpp"

#define ORCUS_DEBUG_XML 1

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

using namespace std;

namespace orcus {

namespace {

class xml_map_sax_handler
{
public:
    void declaration()
    {
    }

    void start_element(const pstring& ns, const pstring& name)
    {
    }

    void end_element(const pstring& ns, const pstring& name)
    {
    }

    void characters(const pstring& val)
    {
    }

    void attribute(const pstring& ns, const pstring& name, const pstring& val)
    {
    }

    void dump(ostream& os)
    {
    }
};

class xml_data_sax_handler
{
public:
    void declaration()
    {
    }

    void start_element(const pstring& ns, const pstring& name)
    {
    }

    void end_element(const pstring& ns, const pstring& name)
    {
    }

    void characters(const pstring& val)
    {
    }

    void attribute(const pstring& ns, const pstring& name, const pstring& val)
    {
    }

    void dump(ostream& os)
    {
    }
};

}

orcus_xml::orcus_xml(model::iface::factory* factory) :
    mp_factory(factory) {}

void orcus_xml::read_map_file(const char* filepath)
{
    cout << "reading map file " << filepath << endl;
    string strm;
    load_file_content(filepath, strm);
    if (strm.empty())
        return;

    xml_map_sax_handler handler;
    sax_parser<xml_map_sax_handler> parser(strm.c_str(), strm.size(), handler);
    parser.parse();
}

void orcus_xml::read_file(const char* filepath)
{
    cout << "reading file " << filepath << endl;
    string strm;
    load_file_content(filepath, strm);
    if (strm.empty())
        return;

    xml_data_sax_handler handler;
    sax_parser<xml_data_sax_handler> parser(strm.c_str(), strm.size(), handler);
    parser.parse();
}

}
