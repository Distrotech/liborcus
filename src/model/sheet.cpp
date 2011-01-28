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
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>

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

void sheet::set_format(row_t row, col_t col, size_t index)
{
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

    if (m_sheet.empty())
        // nothing to print.
        return;

    const shared_strings* sstrings = m_doc.get_shared_strings();

    typedef ::mdds::mixed_type_matrix<string, bool> mx_type;
    mx_type mx(row_count, col_count, ::mdds::matrix_density_sparse_empty);

    // Put all cell values into matrix as string elements first.
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

    // Calculate column widths first.
    mx_type::size_pair_type sp = mx.size();
    vector<size_t> col_widths(sp.second, 0);

    for (size_t r = 0; r < sp.first; ++r)
    {
        for (size_t c = 0; c < sp.second; ++c)
        {
            if (mx.get_type(r, c) == ::mdds::element_empty)
                continue;

            const string* p = mx.get_string(r, c);
            if (col_widths[c] < p->size())
                col_widths[c] = p->size();
        }
    }

    // Create a row separator string;
    ostringstream os;
    os << '+';
    for (size_t i = 0; i < col_widths.size(); ++i)
    {
        os << '-';
        size_t cw = col_widths[i];
        for (size_t i = 0; i < cw; ++i)
            os << '-';
        os << "-+";
    }

    string sep = os.str();

    // Now print to stdout.
    cout << sep << endl;
    for (size_t r = 0; r < row_count; ++r)
    {
        cout << "|";
        for (size_t c = 0; c < col_count; ++c)
        {
            size_t cw = col_widths[c]; // column width
            if (mx.get_type(r, c) == ::mdds::element_empty)
            {
                for (size_t i = 0; i < cw; ++i)
                    cout << ' ';
                cout << "  |";
            }
            else
            {
                const string* s = mx.get_string(r, c);
                cout << ' ' << *s;
                cw -= s->size();
                for (size_t i = 0; i < cw; ++i)
                    cout << ' ';
                cout << " |";
            }
        }
        cout << endl;
        cout << sep << endl;
    }
}

namespace {

template<typename _OSTREAM>
class html_elem
{
public:
    html_elem(_OSTREAM& strm, const char* name, const char* attr = NULL) :
        m_strm(strm), m_name(name)
    {
        if (attr)
            m_strm << '<' << m_name << ' ' << attr << '>';
        else
            m_strm << '<' << m_name << '>';
    }

    ~html_elem()
    {
        m_strm << "</" << m_name << '>';
    }

private:
    _OSTREAM& m_strm;
    const char* m_name;
};

template<typename _OSTREAM>
void print_formatted_text(_OSTREAM& strm, const pstring& text, const shared_strings::format_runs_type& formats)
{
    typedef html_elem<_OSTREAM> elem;

    const char* p_span = "span";

    size_t pos = 0;
    shared_strings::format_runs_type::const_iterator itr = formats.begin(), itr_end = formats.end();
    for (; itr != itr_end; ++itr)
    {
        const shared_strings::format_run& run = *itr;
        if (pos < run.pos)
        {
            // flush unformatted text.
            strm << string(&text[pos], run.pos - pos);
            pos = run.pos;
        }
        
        if (!run.size)
            continue;

        string style = "";
        if (run.bold)
            style += "font-weight: bold;";
        if (run.italic)
            style += "font-style: italic;";

        if (!run.font.empty())
            style += "font-family: " + run.font.str() + ";";

        if (run.font_size)
        {
            ostringstream os;
            os << "font-size: " << run.font_size << "pt;";
            style += os.str();
        }

        if (style.empty())
            strm << string(&text[pos], run.size);
        else
        {
            // Surround the text segment with <span></span> to apply styles.
            style = "style=\"" + style + "\"";
            elem span(strm, p_span, style.c_str());
            strm << string(&text[pos], run.size);
        }

        pos += run.size;
    }

    if (pos < text.size())
    {
        // flush the remaining unformatted text.
        strm << string(&text[pos], text.size() - pos);
    }
}

}

void sheet::dump_html(const string& filepath) const
{
    typedef html_elem<ofstream> elem;

    ofstream file(filepath.c_str());
    if (!file)
    {
        cerr << "failed to create file: " << filepath << endl;
        return;
    }

    const char* p_html  = "html";
    const char* p_table = "table";
    const char* p_tr    = "tr";
    const char* p_td    = "td";
    const char* p_table_attrs      = "style=\"border:1px solid rgb(220,220,220); border-collapse:collapse\"";
    const char* p_empty_cell_attrs = "style=\"border:1px solid rgb(220,220,220); color:white;\"";

    {
        elem root(file, p_html);
    
        if (m_sheet.empty())
            // nothing to print.
            return;
    
        size_t col_count = col_size();

        const shared_strings* sstrings = m_doc.get_shared_strings();
    
        elem table(file, p_table, p_table_attrs);
        sheet_type::const_iterator itr = m_sheet.begin(), itr_end = m_sheet.end();
        
        for (row_t row = 0; itr != itr_end; ++itr, ++row)
        {
            row_t this_row = itr->first;
            for (; row < this_row; ++row)
            {
                // Insert empty row(s).
                elem tr(file, p_tr, p_table_attrs);
                for (col_t col = 0; col < col_count; ++col)
                {
                    elem td(file, p_td, p_empty_cell_attrs);
                    file << '-'; // empty cell.
                }
            }
            const row_type& row_con = *itr->second;
            row_type::const_iterator itr_row = row_con.begin(), itr_row_end = row_con.end();
            elem tr(file, p_tr, p_table_attrs);
            col_t col = 0;
            for (; itr_row != itr_row_end; ++itr_row, ++col)
            {
                for (; col < itr_row->first; ++col)
                {
                    elem td(file, p_td, p_empty_cell_attrs);
                    file << '-'; // empty cell.
                }

                elem td(file, p_td, p_table_attrs);
                const cell& c = itr_row->second;
                switch (c.type)
                {
                    case ct_string:
                    {
                        size_t sindex = static_cast<size_t>(c.value);
                        const pstring& ps = sstrings->get(sindex);
                        const shared_strings::format_runs_type* pformat = sstrings->get_format_runs(sindex);
                        if (pformat)
                            print_formatted_text<ofstream>(file, ps, *pformat);
                        else
                            file << ps;
                    }
                    break;
                    case ct_value:
                    {
                        ostringstream os;
                        os << c.value;
                        file << os.str();
                    }
                    break;
                }
            }

            for (; col < col_count; ++col)
            {
                elem td(file, p_td, p_empty_cell_attrs);
                file << '-'; // empty cell.
            }
        }
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
