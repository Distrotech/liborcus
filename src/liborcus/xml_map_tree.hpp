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
#include "orcus/model/types.hpp"
#include "orcus/exception.hpp"
#include "string_pool.hpp"

#include <ostream>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

/**
 * Tree representing XML-to-sheet mapping for mapped XML import.
 */
class xml_map_tree : boost::noncopyable
{
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

    struct cell_reference : boost::noncopyable
    {
        cell_position pos;

        const char* element_open_begin;
        const char* element_open_end;
        const char* element_close_begin;
        const char* element_close_end;

        cell_reference();
    };

    struct element;
    typedef boost::ptr_vector<element> element_list_type;
    typedef std::vector<const element*> ref_element_list_type;

    struct range_reference : boost::noncopyable
    {
        /**
         * List of elements comprising the fields, in order of appearance from
         * left to right.
         */
        ref_element_list_type elements;

        /**
         * Total number of rows comprising data.  This does not include the
         * label row at the top.
         */
        spreadsheet::row_t row_size;

        range_reference();
    };

    struct field_in_range
    {
        cell_position ref;
        range_reference* range_ref;
        spreadsheet::col_t column_pos;
    };

    typedef std::map<cell_position, range_reference*> range_ref_map_type;

    enum element_type { element_non_leaf, element_cell_ref, element_range_field_ref };

    struct element : boost::noncopyable
    {
        pstring ns;  // TODO: we need to manage namespace externally.
        pstring name;
        element_type type;
        union {
            element_list_type* child_elements;
            cell_reference* cell_ref;
            field_in_range* field_ref;
        };

        element(const pstring& _name, element_type _type);
        ~element();

        const element* get_child(const pstring& _name) const;
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
        const element* push_element(const pstring& name);
        const element* pop_element(const pstring& name);
    };

    xml_map_tree();
    ~xml_map_tree();

    void set_cell_link(const pstring& xpath, const cell_position& ref);
    void append_range_field_link(const pstring& xpath, const cell_position& ref);

    const element* get_link(const pstring& xpath) const;

    walker get_tree_walker() const;

    range_ref_map_type& get_range_references();

private:
    element* get_element(const pstring& xpath, element_type type);

private:
    range_ref_map_type m_field_refs;

    /** pool of element names. */
    string_pool m_names;

    element* m_root;
};

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_position& ref);

bool operator< (const xml_map_tree::cell_position& left, const xml_map_tree::cell_position& right);

}

#endif
