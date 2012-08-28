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

#include "document.hpp"

#include "sheet.hpp"
#include "shared_strings.hpp"
#include "styles.hpp"

#include <ixion/formula.hpp>
#include <ixion/formula_result.hpp>
#include <ixion/matrix.hpp>

#include <iostream>

using namespace std;

namespace orcus { namespace spreadsheet {

namespace {

class find_sheet_by_name : std::unary_function<document::sheet_item, bool>
{
    const pstring& m_name;
public:
    find_sheet_by_name(const pstring& name) : m_name(name) {}
    bool operator() (const document::sheet_item& v) const
    {
        return v.name == m_name;
    }
};

}

document::sheet_item::sheet_item(document& doc, const pstring& _name, sheet_t sheet) :
    name(_name), data(doc, sheet) {}

void document::sheet_item::printer::operator() (const sheet_item& item) const
{
    cout << "---" << endl;
    cout << "Sheet name: " << item.name << endl;
    item.data.dump();
}

document::sheet_item::check_printer::check_printer(std::ostream& os) : m_os(os) {}

void document::sheet_item::check_printer::operator() (const sheet_item& item) const
{
    m_os << "sheet: " << item.name << endl;
    m_os << endl;
    item.data.dump_check(m_os);
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
    mp_strings(NULL),
    mp_styles(new import_styles)
{
    mp_strings = new import_shared_strings(m_context);
}

document::~document()
{
    delete mp_strings;
    delete mp_styles;
}

ixion::matrix document::get_range_value(const ixion::abs_range_t& range) const
{
    if (range.first.sheet < 0 || static_cast<size_t>(range.first.sheet) >= m_sheets.size())
        return ixion::matrix(0, 0);

    size_t sheet_id = static_cast<size_t>(range.first.sheet);
    return m_sheets[sheet_id].data.get_range_value(
        range.first.row, range.first.column, range.last.row, range.last.column);
}

import_shared_strings* document::get_shared_strings()
{
    return mp_strings;
}

const import_shared_strings* document::get_shared_strings() const
{
    return mp_strings;
}

import_styles* document::get_styles()
{
    return mp_styles;
}

const import_styles* document::get_styles() const
{
    return mp_styles;
}

ixion::model_context& document::get_model_context()
{
    return m_context;
}

const ixion::model_context& document::get_model_context() const
{
    return m_context;
}

sheet* document::append_sheet(const pstring& sheet_name)
{
    sheet_t sheet = static_cast<sheet_t>(m_sheets.size());
    m_sheets.push_back(new sheet_item(*this, sheet_name.intern(), sheet));
    m_context.append_sheet(sheet_name.get(), sheet_name.size());
    return &m_sheets.back().data;
}

sheet* document::get_sheet(const pstring& sheet_name)
{
    boost::ptr_vector<sheet_item>::iterator it =
        std::find_if(m_sheets.begin(), m_sheets.end(), find_sheet_by_name(sheet_name));

    if (it == m_sheets.end())
        return NULL;

    return &it->data;
}

void document::calc_formulas()
{
    ixion::iface::model_context& cxt = get_model_context();
    ixion::calculate_cells(cxt, m_dirty_cells, 0);
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

void document::dump_check(ostream& os) const
{
    for_each(m_sheets.begin(), m_sheets.end(), sheet_item::check_printer(os));
}

void document::dump_html(const string& filepath) const
{
    for_each(m_sheets.begin(), m_sheets.end(), sheet_item::html_printer(filepath));
}

ixion::sheet_t document::get_sheet_index(const pstring& name) const
{
    boost::ptr_vector<sheet_item>::const_iterator itr =
        std::find_if(m_sheets.begin(), m_sheets.end(), find_sheet_by_name(name));

    if (itr == m_sheets.end())
        return ixion::invalid_sheet;

    size_t pos = std::distance(m_sheets.begin(), itr);
    return static_cast<ixion::sheet_t>(pos);
}

pstring document::get_sheet_name(ixion::sheet_t sheet) const
{
    if (sheet < 0)
        return pstring();

    size_t pos = static_cast<size_t>(sheet);
    if (pos >= m_sheets.size())
        return pstring();

    return m_sheets[pos].name;
}

void document::insert_dirty_cell(const ixion::abs_address_t& pos)
{
    m_dirty_cells.insert(pos);
}

}}
