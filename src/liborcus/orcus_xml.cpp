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
#include "orcus/model/interface.hpp"

#include "xml_map_tree.hpp"

#define ORCUS_DEBUG_XML 1

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

#include <vector>

using namespace std;

namespace orcus {

namespace {

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
};

}

struct orcus_xml_impl
{
    model::iface::factory* mp_factory;

    /** xml element tree that represents all mapped paths. */
    xml_map_tree m_map_tree;

    /** column position of a field in a current range */
    int m_cur_column_pos;

    xml_map_tree::cell_reference m_cur_range_ref;
};

orcus_xml::orcus_xml(model::iface::factory* factory) :
    mp_impl(new orcus_xml_impl)
{
    mp_impl->mp_factory = factory;
}

void orcus_xml::set_cell_link(const pstring& xpath, const pstring& sheet, model::row_t row, model::col_t col)
{
    mp_impl->m_map_tree.set_cell_link(xpath, xml_map_tree::cell_reference(sheet, row, col));
}

void orcus_xml::start_range(const pstring& sheet, model::row_t row, model::col_t col)
{
    mp_impl->m_cur_range_ref = xml_map_tree::cell_reference(sheet, row, col);
    mp_impl->m_cur_column_pos = 0;
}

void orcus_xml::append_field_link(const pstring& xpath)
{
    mp_impl->m_map_tree.set_range_field_link(xpath, mp_impl->m_cur_range_ref, mp_impl->m_cur_column_pos++);
}

void orcus_xml::commit_range()
{
    mp_impl->m_cur_range_ref = xml_map_tree::cell_reference();
    mp_impl->m_cur_column_pos = -1;
}

void orcus_xml::append_sheet(const pstring& name)
{
    if (name.empty())
        return;

    mp_impl->mp_factory->append_sheet(name.get(), name.size());
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
