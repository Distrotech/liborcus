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

#include "orcus/model/sheet.hpp"
#include "orcus/global.hpp"
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/document.hpp"

#include <iostream>
#include <algorithm>
#include <sstream>

#include <mdds/mixed_type_matrix.hpp>

using namespace std;

namespace orcus { namespace model {

namespace {

struct row_deleter : public unary_function<pair<row_t, sheet::row_type*>, void>
{
    void operator() (const pair<row_t, sheet::row_type*>& r)
    {
        delete r.second;
    }
};

struct colsize_checker : public unary_function<pair<row_t, sheet::row_type*>, void>
{
    colsize_checker() : m_colsize(0) {}
    colsize_checker(const colsize_checker& r) : m_colsize(r.m_colsize) {}

    void operator() (const pair<row_t, sheet::row_type*>& r)
    {
        size_t colsize = r.second->size();
        if (colsize > m_colsize)
            m_colsize = colsize;
    }

    size_t get_colsize() const { return m_colsize; }

private:
    size_t m_colsize;
};

}

sheet::cell::cell() : type(ct_value), value(0.0) {}
sheet::cell::cell(cell_type _type, double _value) : type(_type), value(_value) {}

sheet::sheet(document& doc) :
    m_doc(doc), m_max_row(0), m_max_col(0)
{
}

sheet::~sheet()
{
    for_each(m_sheet.begin(), m_sheet.end(), row_deleter());
}

void sheet::set_string(row_t row, col_t col, size_t sindex)
{
    row_type* p = get_row(row, col);
    p->insert(row_type::value_type(col, cell(ct_string, static_cast<double>(sindex))));
}

void sheet::set_value(row_t row, col_t col, double value)
{
    row_type* p = get_row(row, col);
    p->insert(row_type::value_type(col, cell(ct_value, value)));
}

void sheet::set_cell(row_t row, col_t col, const pstring& val)
{
#if 0 // temporarily disabled
    sheet_type::iterator itr = m_sheet.find(row);
    if (itr == m_sheet.end())
    {
        // This row doesn't exist yet.  Create it.
        pair<sheet_type::iterator, bool> r = m_sheet.insert(sheet_type::value_type(row, new row_type));
        if (!r.second)
            throw general_error("failed to insert a new row instance.");
        itr = r.first;
    }

    row_type* p = itr->second;
    p->insert(row_type::value_type(col, val));
#endif
}

pstring sheet::get_cell(row_t row, col_t col) const
{
    return pstring();
#if 0 // temporarily disabled
    sheet_type::const_iterator itr = m_sheet.find(row);
    if (itr == m_sheet.end())
        return pstring();

    row_type* p = itr->second;
    row_type::const_iterator itr_cell = p->find(col);
    if (itr_cell == p->end())
        return pstring();

    return itr_cell->second;
#endif
}

size_t sheet::row_size() const
{
    if (m_sheet.empty())
        return 0;

    return static_cast<size_t>(m_max_row + 1);
}

size_t sheet::col_size() const
{
    if (m_sheet.empty())
        return 0;

    return static_cast<size_t>(m_max_col + 1);
}

void sheet::dump() const
{
    size_t row_count = row_size();
    size_t col_count = col_size();
    cout << "rows: " << row_count << "  cols: " << col_count << endl;

    const shared_strings* sstrings = m_doc.get_shared_strings();

    typedef ::mdds::mixed_type_matrix<string, bool> mx_type;
    mx_type mx(row_count, col_count, ::mdds::matrix_density_sparse_empty);

    sheet_type::const_iterator itr = m_sheet.begin(), itr_end = m_sheet.end();
    for (; itr != itr_end; ++itr)
    {
        row_t row = itr->first;
        const row_type& row_con = *itr->second;
        row_type::const_iterator itr_row = row_con.begin(), itr_row_end = row_con.end();
        for (; itr_row != itr_row_end; ++itr_row)
        {
            col_t col = itr_row->first;
            const cell& c = itr_row->second;
            switch (c.type)
            {
                case ct_string:
                {
                    size_t sindex = static_cast<size_t>(c.value);
                    const pstring& ps = sstrings->get(sindex);
                    mx.set_string(row, col, new string(ps.get(), ps.size()));
                }
                break;
                case ct_value:
                {
                    ostringstream os;
                    os << c.value;
                    mx.set_string(row, col, new string(os.str()));
                }
                break;
            }
        }
    }

    for (size_t r = 0; r < row_count; ++r)
    {
        for (size_t c = 0; c < col_count; ++c)
        {
            if (mx.get_type(r, c) == ::mdds::element_empty)
                cout << " |";
            else
            {
                const string* s = mx.get_string(r, c);
                cout << *s << "|";
            }
        }
        cout << endl;
    }
}

sheet::row_type* sheet::get_row(row_t row, col_t col)
{
    sheet_type::iterator itr = m_sheet.find(row);
    if (itr == m_sheet.end())
    {
        // This row doesn't exist yet.  Create it.
        pair<sheet_type::iterator, bool> r = m_sheet.insert(sheet_type::value_type(row, new row_type));
        if (!r.second)
            throw general_error("failed to insert a new row instance.");
        itr = r.first;
    }
    if (m_max_row < row)
        m_max_row = row;
    if (m_max_col < col)
        m_max_col = col;

    return itr->second;
}

}}
