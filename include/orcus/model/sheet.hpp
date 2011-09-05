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

#ifndef __ORCUS_MODEL_ODSTABLE_HPP__
#define __ORCUS_MODEL_ODSTABLE_HPP__

#include "orcus/model/interface.hpp"
#include "orcus/pstring.hpp"

#include <mdds/flat_segment_tree.hpp>
#include <ixion/formula_tokens.hpp>

#include <boost/unordered_map.hpp>
#include <map>

namespace orcus { namespace model {

class document;

/**
 * This class represents a single sheet instance in the internal document
 * model.
 */
class sheet : public interface::sheet
{
    static const row_t max_row_limit;
    static const col_t max_col_limit;

    enum cell_type { ct_string, ct_value, ct_formula };

    /**
     * The value of the cell may mean different things in different cell
     * types: for a value cell it's the value, and for a string cell it's an
     * index of the string stored in the cell, to be looked up in the shared
     * string pool.
     */
    struct cell
    {
        cell_type   type;
        double      value;

        cell();
        cell(cell_type _type, double _value);
    };
public:
    typedef ::mdds::flat_segment_tree<col_t, size_t>  segment_col_index_type;
    typedef boost::unordered_map<row_t, segment_col_index_type*> cell_format_type;

    typedef ::std::map<col_t, cell>       row_type;
    typedef ::std::map<row_t, row_type*>  sheet_type;

    sheet(document& doc);
    virtual ~sheet();

    virtual void set_auto(orcus::model::row_t row, orcus::model::col_t col, const char* p, size_t n);
    virtual void set_string(row_t row, col_t col, size_t sindex);
    virtual void set_value(row_t row, col_t col, double value);
    virtual void set_format(row_t row, col_t col, size_t index);
    virtual void set_formula(row_t row, col_t col, formula_grammar_t grammar, const char* p, size_t n);
    virtual void set_shared_formula(row_t row, col_t col, formula_grammar_t grammar, size_t sindex, const char* p, size_t n);
    virtual void set_shared_formula(row_t row, col_t col, size_t sindex);
    virtual void set_formula_result(row_t row, col_t col, const char* p, size_t n);

    row_t row_size() const;
    col_t col_size() const;

    void dump() const;
    void dump_html(const ::std::string& filepath) const;

private:
    void update_size(row_t row, col_t col);
    row_type* get_row(row_t row, col_t col);
    size_t get_cell_format(row_t row, col_t col) const;

private:
    document& m_doc;
    sheet_type m_sheet;  /// group of rows.
    mutable cell_format_type m_cell_formats;
    row_t m_max_row;
    col_t m_max_col;

    /** formula token storage for non-shared formula expressions */
    boost::ptr_vector<ixion::formula_tokens_t> m_formula_tokens;
};

}}

#endif
