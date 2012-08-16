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
#include "string_pool.hpp"

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

/**
 * Tree representing XML-to-sheet mapping for mapped XML import.
 */
class xml_map_tree : boost::noncopyable
{
public:
    struct cell_reference
    {
        pstring sheet;
        model::row_t row;
        model::col_t col;

        cell_reference();
        cell_reference(const pstring& _sheet, model::row_t _row, model::col_t _col);
        cell_reference(const cell_reference& r);
    };

private:
    struct field_in_range
    {
        cell_reference ref;
        int column_pos;
    };

    struct element;
    typedef boost::ptr_vector<element> element_list_type;

    enum element_type { non_leaf, cell_ref, range_field_ref };

    struct element : boost::noncopyable
    {
        pstring name;
        element_type type;
        union {
            element_list_type* child_elements;
            cell_reference* cell_ref;
            field_in_range* field_ref;
        };
    };

public:
    xml_map_tree();
    ~xml_map_tree();

    void set_cell_link(const pstring& xpath, const cell_reference& ref);
    void set_range_field_link(
       const pstring& xpath, const cell_reference& ref, int column_pos);

private:
    /** pool of element names. */
    string_pool m_names;

    element* m_root;
};

}

#endif
