/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#include "orcus/model/document.hpp"
#include "orcus/model/sheet.hpp"
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/styles.hpp"
#include "orcus/model/formula_context.hpp"

#include <ixion/formula.hpp>
#include <ixion/formula_result.hpp>

#include <iostream>

using namespace std;

namespace orcus { namespace model {

document::sheet_item::sheet_item(document& doc, const pstring& _name, sheet_t sheet) :
    name(_name), data(doc, sheet) {}

void document::sheet_item::printer::operator() (const sheet_item& item) const
{
    cout << "---" << endl;
    cout << "Sheet name: " << item.name << endl;
    item.data.dump();
}

document::sheet_item::html_printer::html_printer(const string& filepath) :
    m_filepath(filepath) {}

void document::sheet_item::html_printer::operator() (const sheet_item& item) const
{
    // file path is expected to be a directory.
    string this_file = m_filepath + '/' + item.name.str() + ".html";
    item.data.dump_html(this_file);
}

document::document() :
    mp_strings(new shared_strings),
    mp_styles(new styles),
    mp_formula_cxt(new formula_context(*this))
{
}

document::~document()
{
    delete mp_strings;
    delete mp_styles;
    delete mp_formula_cxt;
}

ixion::abs_address_t document::get_cell_position(const ixion::base_cell* p) const
{
     boost::ptr_vector<sheet_item>::const_iterator itr = m_sheets.begin(), itr_end = m_sheets.end();
     ixion::abs_address_t pos;
     for (; itr != itr_end; ++itr)
     {
         if (itr->data.find_cell_position(p, pos))
             break;
     }
     return pos;
}

const ixion::formula_tokens_t* document::get_formula_tokens(
    sheet_t sheet_id, size_t identifier) const
{
    if (sheet_id < 0)
        return NULL;

    if (sheet_id >= m_sheets.size())
        return NULL;

    const sheet& sh = m_sheets[sheet_id].data;
    return sh.get_formula_tokens(identifier);
}

shared_strings* document::get_shared_strings()
{
    return mp_strings;
}

const shared_strings* document::get_shared_strings() const
{
    return mp_strings;
}

styles* document::get_styles()
{
    return mp_styles;
}

const styles* document::get_styles() const
{
    return mp_styles;
}

formula_context& document::get_formula_context()
{
    return *mp_formula_cxt;
}

const formula_context& document::get_formula_context() const
{
    return *mp_formula_cxt;
}

sheet* document::append_sheet(const pstring& sheet_name)
{
    sheet_t sheet = static_cast<sheet_t>(m_sheets.size());
    m_sheets.push_back(new sheet_item(*this, sheet_name.intern(), sheet));
    return &m_sheets.back().data;
}

void document::calc_formulas()
{
    cout << "dirty cells: " << m_dirty_cells.size() << endl;
    ixion::interface::model_context& cxt = get_formula_context();
    ixion::calculate_cells(cxt, m_dirty_cells, 0);
    ixion::dirty_cells_t::const_iterator itr = m_dirty_cells.begin(), itr_end = m_dirty_cells.end();
    for (; itr != itr_end; ++itr)
    {
        const ixion::formula_cell* cell = *itr;
        const ixion::formula_result* res = cell->get_result_cache();
        if (!res)
            continue;

        cout << cxt.get_cell_name(cell) << ":" << res->str() << endl;
    }
}

void document::dump() const
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "  Document content summary" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    mp_strings->dump();

    cout << "number of sheets: " << m_sheets.size() << endl;
    for_each(m_sheets.begin(), m_sheets.end(), sheet_item::printer());
}

void document::dump_html(const string& filepath) const
{
    for_each(m_sheets.begin(), m_sheets.end(), sheet_item::html_printer(filepath));
}

void document::insert_dirty_cell(ixion::formula_cell* cell)
{
    m_dirty_cells.insert(cell);
}

}}
