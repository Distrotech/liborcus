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
#include "orcus/spreadsheet/interface.hpp"

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
    struct attr
    {
        pstring ns; // TODO: we need to manage namespace externally.
        pstring name;
        pstring val;

        attr(const pstring& _ns, const pstring& _name, const pstring& _val) :
            ns(_ns), name(_name), val(_val) {}
    };

    struct scope
    {
        pstring ns;  // TODO: we need to manage namespace externally.
        pstring name;
        const char* element_open_begin;
        const char* element_open_end;

        xml_map_tree::element_type type;

        scope(const pstring& _ns, const pstring& _name) :
            ns(_ns), name(_name),
            element_open_begin(NULL), element_open_end(NULL),
            type(xml_map_tree::element_unknown) {}
    };

    vector<attr> m_attrs;
    vector<scope> m_scopes;

    spreadsheet::iface::factory& m_factory;
    xml_map_tree::const_element_list_type& m_link_positions;
    xml_map_tree::walker m_map_tree_walker;
    const xml_map_tree::element* mp_current_elem;

public:
    xml_data_sax_handler(
       spreadsheet::iface::factory& factory, xml_map_tree::const_element_list_type& link_positions, const xml_map_tree& map_tree) :
        m_factory(factory),
        m_link_positions(link_positions),
        m_map_tree_walker(map_tree.get_tree_walker()), mp_current_elem(NULL) {}

    void declaration()
    {
        m_attrs.clear();
    }

    void start_element(const sax_parser_element& elem)
    {
        m_scopes.push_back(scope(elem.ns, elem.name));
        scope& cur = m_scopes.back();
        cur.element_open_begin = elem.begin_pos;
        cur.element_open_end = elem.end_pos;

        m_attrs.clear();
        mp_current_elem = m_map_tree_walker.push_element(elem.name);
    }

    void end_element(const sax_parser_element& elem)
    {
        assert(!m_scopes.empty());

        if (mp_current_elem)
        {
            // Store the end element position in stream for linked elements.
            const scope& cur = m_scopes.back();
            switch (mp_current_elem->type)
            {
                case xml_map_tree::element_cell_ref:
                    mp_current_elem->cell_ref->element_open_begin = cur.element_open_begin;
                    mp_current_elem->cell_ref->element_open_end = cur.element_open_end;
                    mp_current_elem->cell_ref->element_close_begin = elem.begin_pos;
                    mp_current_elem->cell_ref->element_close_end = elem.end_pos;
                    m_link_positions.push_back(mp_current_elem);
                break;
                case xml_map_tree::element_range_field_ref:
                break;
                default:
                    ;
            }
        }

        m_scopes.pop_back();
        mp_current_elem = m_map_tree_walker.pop_element(elem.name);
    }

    void characters(const pstring& val)
    {
        if (!mp_current_elem)
            return;

        pstring val_trimmed = val.trim();
        if (val_trimmed.empty())
            return;

        switch (mp_current_elem->type)
        {
            case xml_map_tree::element_cell_ref:
            {
                const xml_map_tree::cell_reference& ref = *mp_current_elem->cell_ref;
                assert(!ref.pos.sheet.empty());

                spreadsheet::iface::sheet* sheet = m_factory.get_sheet(ref.pos.sheet.get(), ref.pos.sheet.size());
                if (sheet)
                    sheet->set_auto(ref.pos.row, ref.pos.col, val_trimmed.get(), val_trimmed.size());
            }
            break;
            case xml_map_tree::element_range_field_ref:
            {
                const xml_map_tree::field_in_range& field = *mp_current_elem->field_ref;
                assert(field.ref);
                assert(!field.ref->pos.sheet.empty());

                if (field.column_pos == 0)
                    ++field.ref->row_size;

                const xml_map_tree::cell_position& pos = field.ref->pos;
                spreadsheet::iface::sheet* sheet = m_factory.get_sheet(pos.sheet.get(), pos.sheet.size());
                if (sheet)
                    sheet->set_auto(
                       pos.row + field.ref->row_size,
                       pos.col + field.column_pos,
                       val_trimmed.get(), val_trimmed.size());
            }
            break;
            case xml_map_tree::element_non_leaf:
            default:
                ;
        }
    }

    void attribute(const pstring& ns, const pstring& name, const pstring& val)
    {
        m_attrs.push_back(attr(ns, name, val));
    }
};

}

struct orcus_xml_impl
{
    spreadsheet::iface::factory* mp_factory;

    /** original xml data stream. */
    string m_data_strm;

    /** xml element tree that represents all mapped paths. */
    xml_map_tree m_map_tree;

    /**
     * Positions of all linked elements, single and range reference alike.
     * Stored link elements must be sorted in order of stream positions, and
     * as such, no linked elements should be nested; there should never be a
     * linked element inside the substructure of another linked element.
     */
    xml_map_tree::const_element_list_type m_link_positions;

    xml_map_tree::cell_position m_cur_range_ref;
};

orcus_xml::orcus_xml(spreadsheet::iface::factory* factory) :
    mp_impl(new orcus_xml_impl)
{
    mp_impl->mp_factory = factory;
}

orcus_xml::~orcus_xml()
{
    delete mp_impl;
}

void orcus_xml::set_cell_link(const pstring& xpath, const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col)
{
    mp_impl->m_map_tree.set_cell_link(xpath, xml_map_tree::cell_position(sheet, row, col));
}

void orcus_xml::start_range(const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col)
{
    mp_impl->m_cur_range_ref = xml_map_tree::cell_position(sheet, row, col);
    mp_impl->m_map_tree.start_range();
}

void orcus_xml::append_field_link(const pstring& xpath)
{
    mp_impl->m_map_tree.append_range_field_link(xpath, mp_impl->m_cur_range_ref);
}

void orcus_xml::commit_range()
{
    mp_impl->m_cur_range_ref = xml_map_tree::cell_position();
    mp_impl->m_map_tree.commit_range();
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
    string& strm = mp_impl->m_data_strm;
    load_file_content(filepath, strm);
    if (strm.empty())
        return;

    // Insert the range headers and reset the row size counters.
    xml_map_tree::range_ref_map_type& range_refs = mp_impl->m_map_tree.get_range_references();
    xml_map_tree::range_ref_map_type::iterator it_ref = range_refs.begin(), it_ref_end = range_refs.end();
    for (; it_ref != it_ref_end; ++it_ref)
    {
        const xml_map_tree::cell_position& ref = it_ref->first;
        xml_map_tree::range_reference& range_ref = *it_ref->second;
        range_ref.row_size = 0; // Reset the row offset.

        spreadsheet::iface::sheet* sheet = mp_impl->mp_factory->get_sheet(ref.sheet.get(), ref.sheet.size());
        if (!sheet)
            continue;

        xml_map_tree::const_element_list_type::const_iterator it = range_ref.elements.begin(), it_end = range_ref.elements.end();
        spreadsheet::row_t row = ref.row;
        spreadsheet::col_t col = ref.col;
        for (; it != it_end; ++it)
        {
            const xml_map_tree::element& e = **it;
            sheet->set_auto(row, col++, e.name.get(), e.name.size());
        }
    }

    // Parse the content xml.
    xml_data_sax_handler handler(*mp_impl->mp_factory, mp_impl->m_link_positions, mp_impl->m_map_tree);
    sax_parser<xml_data_sax_handler> parser(strm.c_str(), strm.size(), handler);
    parser.parse();
}

void orcus_xml::write_file(const char* filepath)
{
    cout << "writing to " << filepath << endl;

    const xml_map_tree::const_element_list_type& links = mp_impl->m_link_positions;
    xml_map_tree::const_element_list_type::const_iterator it = links.begin(), it_end = links.end();
    for (; it != it_end; ++it)
    {
        const xml_map_tree::element& elem = **it;
        switch (elem.type)
        {
            case xml_map_tree::element_cell_ref:
            {
                const char* s = elem.cell_ref->element_open_begin;
                const char* e = elem.cell_ref->element_close_end;
                pstring segment(s, e-s);
                cout << "'" << segment << "'" << endl;
            }
            break;
            default:
                ;
        }
    }
}

}
