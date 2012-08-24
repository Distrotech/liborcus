/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

#ifndef __ORCUS_SPREADSHEET_ODSTABLE_HPP__
#define __ORCUS_SPREADSHEET_ODSTABLE_HPP__

#include "orcus/spreadsheet/interface.hpp"
#include "orcus/pstring.hpp"

#include <mdds/flat_segment_tree.hpp>
#include <ixion/formula_tokens.hpp>
#include <ixion/cell.hpp>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/unordered_map.hpp>
#include <map>
#include <deque>
#include <ostream>

namespace orcus { namespace spreadsheet {

class document;

/**
 * This class represents a single sheet instance in the internal document
 * model.
 */
class sheet : public iface::sheet
{
    static const row_t max_row_limit;
    static const col_t max_col_limit;

public:

    typedef ::mdds::flat_segment_tree<col_t, size_t>  segment_col_index_type;
    typedef boost::unordered_map<row_t, segment_col_index_type*> cell_format_type;

    sheet(document& doc, sheet_t sheet);
    virtual ~sheet();

    virtual void set_auto(orcus::spreadsheet::row_t row, orcus::spreadsheet::col_t col, const char* p, size_t n);
    virtual void set_string(row_t row, col_t col, size_t sindex);
    virtual void set_value(row_t row, col_t col, double value);
    virtual void set_format(row_t row, col_t col, size_t index);
    virtual void set_formula(row_t row, col_t col, formula_grammar_t grammar, const char* p, size_t n);
    virtual void set_shared_formula(
        row_t row, col_t col, formula_grammar_t grammar, size_t sindex,
        const char* p_formula, size_t n_formula, const char* p_range, size_t n_range);
    virtual void set_shared_formula(row_t row, col_t col, size_t sindex);
    virtual void set_formula_result(row_t row, col_t col, const char* p, size_t n);

    row_t row_size() const;
    col_t col_size() const;

    ixion::matrix get_range_value(row_t row1, col_t col1, row_t row2, col_t col2) const;

    void dump() const;
    void dump_check(std::ostream& os) const;
    void dump_html(const ::std::string& filepath) const;

private:
    void update_size(row_t row, col_t col);
    size_t get_cell_format(row_t row, col_t col) const;

private:
    document& m_doc;
    mutable cell_format_type m_cell_formats;
    row_t m_max_row;
    col_t m_max_col;
    const sheet_t m_sheet; /// sheet ID
};

}}

#endif
