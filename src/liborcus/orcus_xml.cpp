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
#include "orcus/sax_ns_parser.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/spreadsheet/export_interface.hpp"
#include "orcus/xml_namespace.hpp"

#include "xml_map_tree.hpp"

#define ORCUS_DEBUG_XML 0

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <fstream>

using namespace std;

namespace orcus {

namespace {

class xml_data_sax_handler
{
    struct attr
    {
        xmlns_id_t ns;
        pstring name;
        pstring val;

        attr(xmlns_id_t _ns, const pstring& _name, const pstring& _val) :
            ns(_ns), name(_name), val(_val) {}
    };

    struct scope
    {
        xmlns_id_t ns;
        pstring name;
        const char* element_open_begin;
        const char* element_open_end;

        xml_map_tree::element_type type;

        scope(xmlns_id_t _ns, const pstring& _name) :
            ns(_ns), name(_name),
            element_open_begin(NULL), element_open_end(NULL),
            type(xml_map_tree::element_unknown) {}
    };

    vector<attr> m_attrs;
    vector<scope> m_scopes;

    spreadsheet::iface::import_factory& m_factory;
    xml_map_tree::const_element_list_type& m_link_positions;
    xml_map_tree::walker m_map_tree_walker;

    const xml_map_tree::element* mp_current_elem;

    bool m_in_range_ref:1;

private:

    const attr* find_attr_by_name(xmlns_id_t ns, const pstring& name)
    {
        vector<attr>::const_iterator it = m_attrs.begin(), it_end = m_attrs.end();
        for (; it != it_end; ++it)
        {
            if (it->ns == ns && it->name == name)
                return &(*it);
        }
        return NULL;
    }

    void set_single_link_cell(const xml_map_tree::cell_reference& ref, const pstring& val)
    {
        spreadsheet::iface::import_sheet* sheet = m_factory.get_sheet(ref.pos.sheet.get(), ref.pos.sheet.size());
        if (sheet)
            sheet->set_auto(ref.pos.row, ref.pos.col, val.get(), val.size());
    }

    void set_field_link_cell(const xml_map_tree::field_in_range& field, const pstring& val)
    {
        assert(field.ref);
        assert(!field.ref->pos.sheet.empty());

        if (field.column_pos == 0)
            ++field.ref->row_size;

        const xml_map_tree::cell_position& pos = field.ref->pos;
        spreadsheet::iface::import_sheet* sheet = m_factory.get_sheet(pos.sheet.get(), pos.sheet.size());
        if (sheet)
            sheet->set_auto(
               pos.row + field.ref->row_size,
               pos.col + field.column_pos,
               val.get(), val.size());
    }

public:
    xml_data_sax_handler(
       spreadsheet::iface::import_factory& factory,
       xml_map_tree::const_element_list_type& link_positions,
       const xml_map_tree& map_tree) :
        m_factory(factory),
        m_link_positions(link_positions),
        m_map_tree_walker(map_tree.get_tree_walker()),
        mp_current_elem(NULL),
        m_in_range_ref(false) {}

    void declaration()
    {
        m_attrs.clear();
    }

    void start_element(const sax_ns_parser_element& elem)
    {
        m_scopes.push_back(scope(elem.ns, elem.name));
        scope& cur = m_scopes.back();
        cur.element_open_begin = elem.begin_pos;
        cur.element_open_end = elem.end_pos;

        mp_current_elem = m_map_tree_walker.push_element(elem.ns, elem.name);
        if (mp_current_elem)
        {
            const xml_map_tree::attribute_store_type& linked_attrs = mp_current_elem->attributes;
            xml_map_tree::attribute_store_type::const_iterator it = linked_attrs.begin(), it_end = linked_attrs.end();
            for (; it != it_end; ++it)
            {
                const xml_map_tree::attribute& linked_attr = *it;
                const attr* p = find_attr_by_name(linked_attr.ns, linked_attr.name);
                if (!p)
                    continue;

                pstring val_trimmed = p->val.trim();
                switch (linked_attr.ref_type)
                {
                    case xml_map_tree::reference_cell:
                        set_single_link_cell(*linked_attr.cell_ref, val_trimmed);
                    break;
                    case xml_map_tree::reference_range_field:
                        set_field_link_cell(*linked_attr.field_ref, val_trimmed);
                    break;
                    default:
                        ;
                }
            }

            if (mp_current_elem->range_parent)
                m_in_range_ref = true;
        }
        m_attrs.clear();
    }

    void end_element(const sax_ns_parser_element& elem)
    {
        assert(!m_scopes.empty());

        if (mp_current_elem)
        {
            // Store the end element position in stream for linked elements.
            const scope& cur = m_scopes.back();
            if (mp_current_elem->ref_type == xml_map_tree::reference_cell ||
                mp_current_elem->range_parent ||
                (!m_in_range_ref && mp_current_elem->unlinked_attribute_anchor()))
            {
                // either single link element, parent of range link elements,
                // or an unlinked attribute anchor outside linked ranges.
                mp_current_elem->stream_pos.open_begin = cur.element_open_begin;
                mp_current_elem->stream_pos.open_end = cur.element_open_end;
                mp_current_elem->stream_pos.close_begin = elem.begin_pos;
                mp_current_elem->stream_pos.close_end = elem.end_pos;
                m_link_positions.push_back(mp_current_elem);
            }

            if (mp_current_elem->range_parent)
                m_in_range_ref = false;
        }

        m_scopes.pop_back();
        mp_current_elem = m_map_tree_walker.pop_element(elem.ns, elem.name);
    }

    void characters(const pstring& val)
    {
        if (!mp_current_elem)
            return;

        pstring val_trimmed = val.trim();
        if (val_trimmed.empty())
            return;

        switch (mp_current_elem->ref_type)
        {
            case xml_map_tree::reference_cell:
                set_single_link_cell(*mp_current_elem->cell_ref, val_trimmed);
            break;
            case xml_map_tree::reference_range_field:
                set_field_link_cell(*mp_current_elem->field_ref, val_trimmed);
            break;
            default:
                ;
        }
    }

    void attribute(const pstring& /*name*/, const pstring& /*val*/)
    {
        // Ignore attributes in XML declaration.
    }

    void attribute(const sax_ns_parser_attribute& at)
    {
        m_attrs.push_back(attr(at.ns, at.name, at.value));
    }
};

/**
 * Used in write_range_reference_group().
 */
struct scope : boost::noncopyable
{
    const xml_map_tree::element& element;
    xml_map_tree::element_store_type::const_iterator current_child_pos;
    xml_map_tree::element_store_type::const_iterator end_child_pos;
    bool opened:1;

    scope(const xml_map_tree::element& _elem) :
        element(_elem), opened(false)
    {
        current_child_pos = end_child_pos;

        if (element.elem_type == xml_map_tree::element_unlinked)
        {
            current_child_pos = element.child_elements->begin();
            end_child_pos = element.child_elements->end();
        }
    }
};

typedef boost::ptr_vector<scope> scopes_type;

void write_opening_element(
    ostream& os, const xml_map_tree::element& elem, const xml_map_tree::range_reference& ref,
    const spreadsheet::iface::export_sheet& sheet, spreadsheet::row_t current_row, bool self_close)
{
    if (elem.attributes.empty())
    {
        // This element has no linked attributes. Just write the element name and be done with it.
        os << '<' << elem.name << '>';
        return;
    }

    // Element has one or more linked attributes.

    os << '<' << elem.name;

    xml_map_tree::attribute_store_type::const_iterator it = elem.attributes.begin(), it_end = elem.attributes.end();
    for (; it != it_end; ++it)
    {
        const xml_map_tree::attribute& attr = *it;
        if (attr.ref_type != xml_map_tree::reference_range_field)
            // In theory this should never happen but it won't hurt to check.
            continue;

        os << ' ' << attr.name << "=\"";
        sheet.write_string(os, ref.pos.row + 1 + current_row, ref.pos.col + attr.field_ref->column_pos);
        os << "\"";
    }

    if (self_close)
        os << '/';

    os << '>';
}

void write_opening_element(
    ostream& os, const xml_map_tree::element& elem, const spreadsheet::iface::export_factory& fact, bool self_close)
{
    os << '<' << elem.name;
    xml_map_tree::attribute_store_type::const_iterator it = elem.attributes.begin(), it_end = elem.attributes.end();
    for (; it != it_end; ++it)
    {
        const xml_map_tree::attribute& attr = *it;
        if (attr.ref_type != xml_map_tree::reference_cell)
            // We should only see single linked cell here, as all
            // field links are handled by the range parent above.
            continue;

        const xml_map_tree::cell_position& pos = attr.cell_ref->pos;

        const spreadsheet::iface::export_sheet* sheet =
            fact.get_sheet(pos.sheet.get(), pos.sheet.size());
        if (!sheet)
            continue;

        os << ' ' << attr.name << "=\"";
        sheet->write_string(os, pos.row, pos.col);
        os << "\"";
    }

    if (self_close)
        os << '/';

    os << '>';
}

/**
 * Write to the output stream a single range reference.
 *
 * @param os output stream.
 * @param root root map tree element representing the root of a single range
 *             reference.
 * @param ref range reference data.
 * @param factory export factory instance.
 */
void write_range_reference_group(
   ostream& os, const xml_map_tree::element& root, const xml_map_tree::range_reference& ref,
   const spreadsheet::iface::export_factory& factory)
{
    const spreadsheet::iface::export_sheet* sheet = factory.get_sheet(ref.pos.sheet.get(), ref.pos.sheet.size());
    if (!sheet)
        return;

    scopes_type scopes;
    for (spreadsheet::row_t current_row = 0; current_row < ref.row_size; ++current_row)
    {
        scopes.push_back(new scope(root)); // root element

        while (!scopes.empty())
        {
            bool new_scope = false;

            scope& cur_scope = scopes.back();

            // Self-closing element has no child elements nor content.
            bool self_close =
                (cur_scope.current_child_pos == cur_scope.end_child_pos) &&
                (cur_scope.element.ref_type != xml_map_tree::reference_range_field);

            if (!cur_scope.opened)
            {
                // Write opening element of this scope only on the 1st entrance.
                write_opening_element(os, cur_scope.element, ref, *sheet, current_row, self_close);
                cur_scope.opened = true;
            }

            if (self_close)
            {
                scopes.pop_back();
                continue;
            }

            // Go though all child elements.
            for (; cur_scope.current_child_pos != cur_scope.end_child_pos; ++cur_scope.current_child_pos)
            {
                const xml_map_tree::element& child_elem = *cur_scope.current_child_pos;
                if (child_elem.elem_type == xml_map_tree::element_unlinked)
                {
                    // This is a non-leaf element.  Push a new scope with this
                    // element and re-start the loop.
                    ++cur_scope.current_child_pos;
                    scopes.push_back(new scope(child_elem));
                    new_scope = true;
                    break;
                }

                // This is a leaf element.  This must be a field link element.
                if (child_elem.ref_type == xml_map_tree::reference_range_field)
                {
                    write_opening_element(os, child_elem, ref, *sheet, current_row, false);
                    sheet->write_string(os, ref.pos.row + 1 + current_row, ref.pos.col + child_elem.field_ref->column_pos);
                    os << "</" << child_elem.name << ">";
                }
            }

            if (new_scope)
                // Re-start the loop with a new scope.
                continue;

            // Write content of this element before closing it (if it's linked).
            if (scopes.back().element.ref_type == xml_map_tree::reference_range_field)
                sheet->write_string(
                    os, ref.pos.row + 1 + current_row, ref.pos.col + scopes.back().element.field_ref->column_pos);

            // Close this element for good, and exit the current scope.
            os << "</" << scopes.back().element.name << ">";
            scopes.pop_back();
        }
    }
}

/**
 * Write to an output stream the sub-structure comprising one or more range
 * references.
 *
 * @param os output stream
 * @param elem_top topmost element in the range reference sub-structure.
 */
void write_range_reference(ostream& os, const xml_map_tree::element& elem_top, const spreadsheet::iface::export_factory& factory)
{
    // Top element is expected to have one or more child elements, and each
    // child element represents a separate database range.
    if (elem_top.elem_type != xml_map_tree::element_unlinked)
        return;

    assert(elem_top.child_elements);

    if (elem_top.child_elements->empty())
        return;

    // TODO: For now, we assume that there is only one child element under the
    // range ref parent.
    write_range_reference_group(
       os, *elem_top.child_elements->begin(), *elem_top.range_parent, factory);
}

struct less_by_opening_elem_pos : std::binary_function<xml_map_tree::element*, xml_map_tree::element*, bool>
{
    bool operator() (const xml_map_tree::element* left, const xml_map_tree::element* right) const
    {
        return left->stream_pos.open_begin < right->stream_pos.open_begin;
    }
};

}

struct orcus_xml_impl
{
    spreadsheet::iface::import_factory* mp_import_factory;
    spreadsheet::iface::export_factory* mp_export_factory;

    /** original xml data stream. */
    string m_data_strm;

    /** xml namespace repository for the whole session. */
    xmlns_repository& m_ns_repo;

    /** xml namespace context  */
    xmlns_context m_ns_cxt_map;

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

    orcus_xml_impl(xmlns_repository& ns_repo) : m_ns_repo(ns_repo), m_ns_cxt_map(ns_repo.create_context()), m_map_tree(m_ns_cxt_map) {}
};

orcus_xml::orcus_xml(xmlns_repository& ns_repo, spreadsheet::iface::import_factory* im_fact, spreadsheet::iface::export_factory* ex_fact) :
    mp_impl(new orcus_xml_impl(ns_repo))
{
    mp_impl->mp_import_factory = im_fact;
    mp_impl->mp_export_factory = ex_fact;
}

orcus_xml::~orcus_xml()
{
    delete mp_impl;
}

void orcus_xml::set_namespace_alias(const pstring& alias, const pstring& uri)
{
    mp_impl->m_map_tree.set_namespace_alias(alias, uri);
}

void orcus_xml::set_cell_link(const pstring& xpath, const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col)
{
    pstring sheet_safe = mp_impl->m_map_tree.intern_string(sheet);
    mp_impl->m_map_tree.set_cell_link(xpath, xml_map_tree::cell_position(sheet_safe, row, col));
}

void orcus_xml::start_range(const pstring& sheet, spreadsheet::row_t row, spreadsheet::col_t col)
{
    pstring sheet_safe = mp_impl->m_map_tree.intern_string(sheet);
    mp_impl->m_cur_range_ref = xml_map_tree::cell_position(sheet_safe, row, col);
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

    mp_impl->mp_import_factory->append_sheet(name.get(), name.size());
}

void orcus_xml::read_file(const char* filepath)
{
#if ORCUS_DEBUG_XML
    cout << "reading file " << filepath << endl;
#endif
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

        spreadsheet::iface::import_sheet* sheet =
            mp_impl->mp_import_factory->get_sheet(ref.sheet.get(), ref.sheet.size());

        if (!sheet)
            continue;

        xml_map_tree::const_linkable_list_type::const_iterator it = range_ref.field_nodes.begin(), it_end = range_ref.field_nodes.end();
        spreadsheet::row_t row = ref.row;
        spreadsheet::col_t col = ref.col;
        for (; it != it_end; ++it)
        {
            const xml_map_tree::linkable& e = **it;
            sheet->set_auto(row, col++, e.name.get(), e.name.size());
        }
    }

    // Parse the content xml.
    xmlns_context ns_cxt = mp_impl->m_ns_repo.create_context(); // new ns context for the content xml stream.
    xml_data_sax_handler handler(
       *mp_impl->mp_import_factory, mp_impl->m_link_positions, mp_impl->m_map_tree);

    sax_ns_parser<xml_data_sax_handler> parser(strm.c_str(), strm.size(), ns_cxt, handler);
    parser.parse();
}

void orcus_xml::write_file(const char* filepath)
{
    if (!mp_impl->mp_export_factory)
        // We can't export data witout export factory.
        return;

    if (mp_impl->m_data_strm.empty())
        // Original xml stream is missing.  We need it.
        return;

    xml_map_tree::const_element_list_type& links = mp_impl->m_link_positions;
    if (links.empty())
        // nothing to write.
        return;

    // Sort all link position by opening element positions.
    std::sort(links.begin(), links.end(), less_by_opening_elem_pos());

#if ORCUS_DEBUG_XML
    cout << "writing to " << filepath << endl;
#endif
    ofstream file(filepath);

    if (!file)
        throw general_error("Failed to create output file.");

    spreadsheet::iface::export_factory& fact = *mp_impl->mp_export_factory;
    xml_map_tree::const_element_list_type::const_iterator it = links.begin(), it_end = links.end();
    const char* begin_pos = &mp_impl->m_data_strm[0];
    for (; it != it_end; ++it)
    {
        const xml_map_tree::element& elem = **it;
        if (elem.ref_type == xml_map_tree::reference_cell)
        {
            // Single cell link
            const xml_map_tree::cell_position& pos = elem.cell_ref->pos;

            const spreadsheet::iface::export_sheet* sheet =
                fact.get_sheet(pos.sheet.get(), pos.sheet.size());
            if (!sheet)
                continue;

            const char* open_begin = elem.stream_pos.open_begin;
            const char* close_begin = elem.stream_pos.close_begin;
            const char* close_end = elem.stream_pos.close_end;

            file << pstring(begin_pos, open_begin-begin_pos); // stream since last linked element.
            write_opening_element(file, elem, fact, false);
            sheet->write_string(file, pos.row, pos.col);
            file << pstring(close_begin, close_end-close_begin); // closing element.
            begin_pos = close_end;
        }
        else if (elem.range_parent)
        {
            // Range link
            const xml_map_tree::range_reference& ref = *elem.range_parent;
            const xml_map_tree::cell_position& pos = ref.pos;

            const spreadsheet::iface::export_sheet* sheet =
                fact.get_sheet(pos.sheet.get(), pos.sheet.size());
            if (!sheet)
                continue;

            const char* open_begin = elem.stream_pos.open_begin;
            const char* close_begin = elem.stream_pos.close_begin;
            const char* close_end = elem.stream_pos.close_end;

            file << pstring(begin_pos, open_begin-begin_pos); // stream since last linked element.
            write_opening_element(file, elem, fact, false);
            write_range_reference(file, elem, fact);
            file << pstring(close_begin, close_end-close_begin); // closing element.
            begin_pos = close_end;
        }
        else if (elem.unlinked_attribute_anchor())
        {
            // Element is not linked but has one or more attributes that are
            // linked.  Here, only write the opening element with attributes.

            const char* open_begin = elem.stream_pos.open_begin;
            const char* open_end = elem.stream_pos.open_end;

            bool self_close = open_begin == elem.stream_pos.close_begin;

            file << pstring(begin_pos, open_begin-begin_pos); // stream since last linked element.
            write_opening_element(file, elem, fact, self_close);
            begin_pos = open_end;
        }
        else
            throw general_error("Non-link element type encountered.");
    }

    // Flush the remaining stream.
    const char* strm_end = &mp_impl->m_data_strm[mp_impl->m_data_strm.size()-1];
    file << pstring(begin_pos, strm_end-begin_pos);
}

}
