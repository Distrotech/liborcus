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

#include "pstring.hpp"
#include "model/types.hpp"

#include <vector>

namespace orcus {

/**
 * Tree representing XML-to-sheet mapping for mapped XML import.
 */
class xml_map_tree
{
    struct cell_reference
    {
        pstring sheet;
        model::row_t row;
        model::col_t col;
    };

    struct field_in_range
    {
        cell_reference ref;
        int column_pos;
    };

    struct element;
    typedef std::vector<element> element_list_type;

    enum element_type { non_leaf, cell_ref, range_field_ref };

    struct element
    {
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

private:

};

}

#endif
