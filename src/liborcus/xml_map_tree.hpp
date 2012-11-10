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

#ifndef __ORCUS_XML_MAP_TREE_HPP__
#define __ORCUS_XML_MAP_TREE_HPP__

#include "orcus/pstring.hpp"
#include "orcus/spreadsheet/types.hpp"
#include "orcus/exception.hpp"
#include "orcus/types.hpp"
#include "orcus/xml_namespace.hpp"
#include "string_pool.hpp"

#include <ostream>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

class xmlns_context;

/**
 * Tree representing XML-to-sheet mapping for mapped XML import.
 */
class xml_map_tree : boost::noncopyable
{
    xml_map_tree(); // disabled

public:
    /**
     * Error indicating improper xpath syntax.
     */
    class xpath_error : public general_error
    {
    public:
        xpath_error(const std::string& msg);
    };

    /**
     * A single cell position.  Used both for single cell as well as range
     * links.  For a range link, this represents the upper-left cell of a
     * range.
     */
    struct cell_position
    {
        pstring sheet;
        spreadsheet::row_t row;
        spreadsheet::col_t col;

        cell_position();
        cell_position(const pstring& _sheet, spreadsheet::row_t _row, spreadsheet::col_t _col);
        cell_position(const cell_position& r);
    };

    /**
     * Positions of opening and closing elements in xml stream.
     */
    struct element_position
    {
        const char* open_begin;
        const char* open_end;
        const char* close_begin;
        const char* close_end;

        element_position();
    };

    struct cell_reference : boost::noncopyable
    {
        cell_position pos;
        cell_reference();
    };

    struct element;
    struct linkable;
    typedef boost::ptr_vector<element> element_store_type;
    typedef std::vector<element*> element_list_type;
    typedef std::vector<const element*> const_element_list_type;
    typedef std::vector<const linkable*> const_linkable_list_type;

    struct range_reference : boost::noncopyable
    {
        cell_position pos;

        /**
         * List of elements comprising the fields, in order of appearance from
         * left to right.
         */
        const_linkable_list_type field_nodes;

        /**
         * Total number of rows comprising data.  This does not include the
         * label row at the top.
         */
        spreadsheet::row_t row_size;

        range_reference(const cell_position& _pos);
    };

    struct field_in_range
    {
        range_reference* ref;
        spreadsheet::col_t column_pos;
    };

    typedef std::map<cell_position, range_reference*> range_ref_map_type;

    enum linkable_node_type { node_unknown, node_element, node_attribute };
    enum reference_type { reference_unknown, reference_cell, reference_range_field };
    enum element_type { element_unknown, element_linked, element_unlinked };

    struct linkable : boost::noncopyable
    {
        xmlns_id_t ns;
        pstring name;
        linkable_node_type node_type;

        linkable(xmlns_id_t _ns, const pstring& _name, linkable_node_type _node_type);
    };

    struct attribute : public linkable
    {
        reference_type ref_type;
        union {
            cell_reference* cell_ref;
            field_in_range* field_ref;
        };

        attribute(xmlns_id_t _ns, const pstring& _name, reference_type _ref_type);
        ~attribute();
    };

    typedef boost::ptr_vector<attribute> attribute_store_type;

    struct element : public linkable
    {
        element_type elem_type;
        reference_type ref_type;

        union {
            element_store_type* child_elements;
            cell_reference* cell_ref;
            field_in_range* field_ref;
        };

        mutable element_position stream_pos;

        attribute_store_type attributes;

        /**
         * Points to a range reference instance of which this element is a
         * parent. NULL if this element is not a parent element of any range
         * reference.
         */
        range_reference* range_parent;

        element(xmlns_id_t _ns, const pstring& _name, element_type _elem_type, reference_type _ref_type);
        ~element();

        const element* get_child(xmlns_id_t _ns, const pstring& _name) const;

        /**
         * Unlinked attribute anchor is an element that's not linked but has
         * one or more attributes that are linked.
         *
         * @return true if the element is an unlinked attribute anchor, false
         *         otherwise.
         */
        bool unlinked_attribute_anchor() const;
    };

public:

    /**
     * Wrapper class to allow walking through the element tree.
     */
    class walker
    {
        typedef std::vector<const element*> element_stack_type;
        const xml_map_tree& m_parent;
        element_stack_type m_stack;
        int m_content_depth;
    public:
        walker(const xml_map_tree& parent);
        walker(const walker& r);

        void reset();
        const element* push_element(xmlns_id_t ns, const pstring& name);
        const element* pop_element(xmlns_id_t ns, const pstring& name);
    };

    xml_map_tree(const xmlns_context& xmlns_cxt);
    ~xml_map_tree();

    void set_namespace_alias(const pstring& alias, const pstring& uri);

    void set_cell_link(const pstring& xpath, const cell_position& ref);

    void start_range();
    void append_range_field_link(const pstring& xpath, const cell_position& pos);
    void commit_range();

    const linkable* get_link(const pstring& xpath) const;

    walker get_tree_walker() const;

    range_ref_map_type& get_range_references();

    pstring intern_string(const pstring& str);

private:
    linkable* get_element_stack(const pstring& xpath, reference_type type, element_list_type& elem_stack);

private:
    xmlns_context m_xmlns_cxt;

    /**
     * Element stack of current range parent element. This is used to
     * determine a common parent element for all field links of a current
     * range reference.
     */
    element_list_type m_cur_range_parent;

    range_reference* mp_cur_range_ref;

    /**
     * All range references present in the tree.  This container manages the
     * life cycles of stored range references.
     */
    range_ref_map_type m_field_refs;

    /** pool of element names. */
    string_pool m_names;

    element* mp_root;
};

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_position& ref);

bool operator< (const xml_map_tree::cell_position& left, const xml_map_tree::cell_position& right);

}

#endif
