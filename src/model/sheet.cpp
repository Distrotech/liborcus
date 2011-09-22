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
#include "orcus/model/styles.hpp"
#include "orcus/global.hpp"
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/document.hpp"
#include "orcus/model/formula_context.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cassert>
#include <memory>

#include <mdds/mixed_type_matrix.hpp>
#include <ixion/formula.hpp>

using namespace std;

namespace orcus { namespace model {

namespace {

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

const row_t sheet::max_row_limit = 1048575;
const col_t sheet::max_col_limit = 1023;

sheet::sheet(document& doc, sheet_t sheet) :
    m_doc(doc), m_max_row(0), m_max_col(0), m_sheet(sheet)
{
}

sheet::~sheet()
{
    for_each(m_rows.begin(), m_rows.end(), delete_map_object<rows_type>());
    for_each(m_cell_formats.begin(), m_cell_formats.end(),
             delete_map_object<cell_format_type>());
}

void sheet::set_auto(row_t row, col_t col, const char* p, size_t n)
{
    shared_strings* ss = m_doc.get_shared_strings();
    assert(ss);
    size_t si = ss->add(p, n);
    set_string(row, col, si);
}

void sheet::set_string(row_t row, col_t col, size_t sindex)
{
    row_type* p = get_row(row, col);
    p->insert(col, new ixion::string_cell(sindex));
}

void sheet::set_value(row_t row, col_t col, double value)
{
    row_type* p = get_row(row, col);
    p->insert(col, new ixion::numeric_cell(value));
}

void sheet::set_format(row_t row, col_t col, size_t index)
{
    cell_format_type::iterator itr = m_cell_formats.find(row);
    if (itr == m_cell_formats.end())
    {
        pair<cell_format_type::iterator, bool> r =
            m_cell_formats.insert(
                cell_format_type::value_type(
                    row, new segment_col_index_type(0, max_col_limit, 0)));

        if (!r.second)
        {
            cerr << "insertion of new cell format container failed!" << endl;
            return;
        }
        itr = r.first;
    }

    segment_col_index_type& con = *itr->second;
    con.insert_back(col, col+1, index);

    update_size(row, col);
}

void sheet::set_formula(row_t row, col_t col, formula_grammar_t grammar,
                        const char* p, size_t n)
{
    // Tokenize the formula string and store it.
    auto_ptr<ixion::formula_tokens_t> tokens(new ixion::formula_tokens_t);
    const formula_context& cxt = m_doc.get_formula_context();
    ixion::abs_address_t pos(m_sheet, row, col);
    ixion::parse_formula_string(cxt, pos, p, n, *tokens);
    m_formula_tokens.push_back(tokens);
    size_t index = m_formula_tokens.size() - 1;

    row_type* row_store = get_row(row, col);
    row_store->insert(col, new ixion::formula_cell(index));
}

void sheet::set_shared_formula(row_t row, col_t col, formula_grammar_t grammar,
                               size_t sindex, const char* p, size_t n)
{
}

void sheet::set_shared_formula(row_t row, col_t col, size_t sindex)
{
}

void sheet::set_formula_result(row_t row, col_t col, const char* p, size_t n)
{
}

row_t sheet::row_size() const
{
    if (m_rows.empty())
        return 0;

    return m_max_row + 1;
}

col_t sheet::col_size() const
{
    if (m_rows.empty())
        return 0;

    return m_max_col + 1;
}

void sheet::dump() const
{
    size_t row_count = row_size();
    size_t col_count = col_size();
    cout << "rows: " << row_count << "  cols: " << col_count << endl;

    if (m_rows.empty())
        // nothing to print.
        return;

    const shared_strings* sstrings = m_doc.get_shared_strings();

    typedef ::mdds::mixed_type_matrix<string, bool> mx_type;
    mx_type mx(row_count, col_count, ::mdds::matrix_density_sparse_empty);

    // Put all cell values into matrix as string elements first.
    rows_type::const_iterator itr = m_rows.begin(), itr_end = m_rows.end();
    for (; itr != itr_end; ++itr)
    {
        row_t row = itr->first;
        const row_type& row_con = *itr->second;
        row_type::const_iterator itr_row = row_con.begin(), itr_row_end = row_con.end();
        for (; itr_row != itr_row_end; ++itr_row)
        {
            col_t col = itr_row->first;
            const ixion::base_cell& c = *itr_row->second;
            switch (c.get_celltype())
            {
                case ixion::celltype_string:
                {
                    size_t sindex = c.get_identifier();
                    const pstring& ps = sstrings->get(sindex);
                    mx.set_string(row, col, new string(ps.get(), ps.size()));
                }
                break;
                case ixion::celltype_numeric:
                {
                    ostringstream os;
                    os << c.get_value();
                    mx.set_string(row, col, new string(os.str()));
                }
                break;
                case ixion::celltype_formula:
                {
                    // TODO : print the formula result.  For now, let's just
                    // print the formula expression.
                    size_t index = c.get_identifier();
                    if (index < m_formula_tokens.size())
                    {
                        const ixion::formula_tokens_t& t = m_formula_tokens[index];
                        auto_ptr<string> str(new string);
                        ixion::abs_address_t pos(row, col, 0);
                        ixion::print_formula_tokens(
                            m_doc.get_formula_context(), pos, t, *str);
                        mx.set_string(row, col, str.release());
                    }
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
            m_strm << '<' << m_name << " style=\"" << attr << "\">";
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

void build_style_string(string& str, const styles& styles, const styles::xf& fmt)
{
    ostringstream os;
    if (fmt.font)
    {
        const styles::font* p = styles.get_font(fmt.font);
        if (p)
        {
            if (!p->name.empty())
                os << "font-family: " << p->name << ";";
            if (p->size)
                os << "font-size: " << p->size << "pt;";
            if (p->bold)
                os << "font-weight: bold;";
            if (p->italic)
                os << "font-style: italic;";
        }
    }
    if (fmt.fill)
    {
        const styles::fill* p = styles.get_fill(fmt.fill);
        if (p)
        {
            if (p->pattern_type == "solid")
            {
                const styles::color& r = p->fg_color;
                os << "background-color: rgb(" << r.red << "," << r.green << "," << r.blue << ");";
            }
        }
    }
    str += os.str();
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
    const char* p_table_attrs      = "border:1px solid rgb(220,220,220); border-collapse:collapse;";
    const char* p_empty_cell_attrs = "border:1px solid rgb(220,220,220); color:white;";

    {
        elem root(file, p_html);

        if (m_rows.empty())
            // nothing to print.
            return;

        size_t col_count = col_size();

        const shared_strings* sstrings = m_doc.get_shared_strings();

        elem table(file, p_table, p_table_attrs);
        rows_type::const_iterator itr = m_rows.begin(), itr_end = m_rows.end();
        row_t row = 0;
        for (; itr != itr_end; ++itr, ++row)
        {
            row_t this_row = itr->first;
            for (; row < this_row; ++row)
            {
                // Insert empty row(s).
                elem tr(file, p_tr, p_table_attrs);
                for (col_t col = 0; col < col_count; ++col)
                {
                    size_t xf = get_cell_format(row, col);
                    string style;
                    if (xf)
                    {
                        // Apply cell format.
                        styles* p_styles = m_doc.get_styles();
                        const styles::xf* fmt = p_styles->get_cell_xf(xf);
                        if (fmt)
                            build_style_string(style, *p_styles, *fmt);
                    }
                    style += p_empty_cell_attrs;
                    elem td(file, p_td, style.c_str());
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
                    string style;
                    style += p_empty_cell_attrs;
                    elem td(file, p_td, style.c_str());
                    file << '-'; // empty cell.
                }

                size_t xf = get_cell_format(row, col);
                string style = p_table_attrs;
                if (xf)
                {
                    // Apply cell format.
                    styles* p_styles = m_doc.get_styles();
                    const styles::xf* fmt = p_styles->get_cell_xf(xf);
                    if (fmt)
                        build_style_string(style, *p_styles, *fmt);
                }

                elem td(file, p_td, style.c_str());
                const ixion::base_cell& c = *itr_row->second;
                ostringstream os;
                switch (c.get_celltype())
                {
                    case ixion::celltype_string:
                    {
                        size_t sindex = c.get_identifier();
                        const pstring& ps = sstrings->get(sindex);
                        const shared_strings::format_runs_type* pformat = sstrings->get_format_runs(sindex);
                        if (pformat)
                            print_formatted_text<ostringstream>(os, ps, *pformat);
                        else
                            os << ps;
                    }
                    break;
                    case ixion::celltype_numeric:
                        os << c.get_value();
                    break;
                    case ixion::celltype_formula:
                        // TODO : print formula result.
                        os << "formula";
                    break;
                }

                file << os.str();
            }

            for (; col < col_count; ++col)
            {
                string style;
                style += p_empty_cell_attrs;
                style += "\"";
                elem td(file, p_td, style.c_str());
                file << '-'; // empty cell.
            }
        }

        row_t row_count = row_size();
        for (; row < row_count; ++row)
        {
            // Insert empty row(s).
            elem tr(file, p_tr, p_table_attrs);
            for (col_t col = 0; col < col_count; ++col)
            {
                size_t xf = get_cell_format(row, col);
                string style;
                if (xf)
                {
                    // Apply cell format.
                    styles* p_styles = m_doc.get_styles();
                    const styles::xf* fmt = p_styles->get_cell_xf(xf);
                    if (fmt)
                        build_style_string(style, *p_styles, *fmt);
                }
                style += p_empty_cell_attrs;
                elem td(file, p_td, style.c_str());
                file << '-'; // empty cell.
            }
        }
    }
}

void sheet::update_size(row_t row, col_t col)
{
    if (m_max_row < row)
        m_max_row = row;
    if (m_max_col < col)
        m_max_col = col;
}

sheet::row_type* sheet::get_row(row_t row, col_t col)
{
    rows_type::iterator itr = m_rows.find(row);
    if (itr == m_rows.end())
    {
        // This row doesn't exist yet.  Create it.
        pair<rows_type::iterator, bool> r = m_rows.insert(rows_type::value_type(row, new row_type));
        if (!r.second)
            throw general_error("failed to insert a new row instance.");
        itr = r.first;
    }
    update_size(row, col);

    return itr->second;
}

size_t sheet::get_cell_format(row_t row, col_t col) const
{
    cell_format_type::const_iterator itr = m_cell_formats.find(row);
    if (itr == m_cell_formats.end())
        return 0;

    segment_col_index_type& con = *itr->second;
    if (!con.is_tree_valid())
        con.build_tree();

    size_t index;
    if (!con.search_tree(col, index))
        return 0;

    return index;
}

}}
