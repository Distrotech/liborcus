/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef ORCUS_ODF_STYLES_HPP
#define ORCUS_ODF_STYLES_HPP

#include "orcus/pstring.hpp"
#include "orcus/measurement.hpp"

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>

namespace orcus {

enum odf_style_family
{
    style_family_unknown = 0,
    style_family_table_column,
    style_family_table_row,
    style_family_table_cell,
    style_family_table,
    style_family_graphic,
    style_family_paragraph,
    style_family_text
};

/**
 * Each instance of this class represents a single <style:style> entry.
 */
struct odf_style : boost::noncopyable
{
    struct column
    {
        length_t width;
    };

    struct row
    {
        length_t height;
    };

    struct cell
    {
        size_t font;
    };

    struct table
    {
    };

    struct graphic
    {
    };

    struct paragraph
    {
    };

    struct text
    {
        size_t font;
    };

    pstring name;
    odf_style_family family;

    union {
        column* column_data;
        row* row_data;
        table* table_data;
        cell* cell_data;
        graphic* graphic_data;
        paragraph* paragraph_data;
        text* text_data;
    };

    odf_style();
    odf_style(const pstring& _name, odf_style_family _family);

    ~odf_style();
};

typedef boost::ptr_map<pstring, odf_style> odf_styles_map_type;

}

#endif
