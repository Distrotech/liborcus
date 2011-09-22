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
#include "orcus/model/shared_strings.hpp"
#include "orcus/model/styles.hpp"
#include "orcus/model/formula_context.hpp"

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

}}
