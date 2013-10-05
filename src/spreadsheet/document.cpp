/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/document.hpp"

#include "orcus/spreadsheet/global_settings.hpp"
#include "orcus/spreadsheet/sheet.hpp"
#include "orcus/spreadsheet/shared_strings.hpp"
#include "orcus/spreadsheet/styles.hpp"

#include "orcus/pstring.hpp"
#include "orcus/types.hpp"
#include "orcus/string_pool.hpp"

#include <ixion/formula.hpp>
#include <ixion/formula_result.hpp>
#include <ixion/matrix.hpp>
#include <ixion/model_context.hpp>

#include <iostream>
#include <fstream>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

namespace orcus { namespace spreadsheet {

namespace {

/**
 * Single sheet entry which consists of a sheet name and a sheet data.
 * Use the printer function object to print sheet content with for_each
 * function.
 */
struct sheet_item : private boost::noncopyable
{
    pstring name;
    sheet   data;
    sheet_item(document& doc, const pstring& _name, sheet_t sheet_index, row_t row_size, col_t col_size);

    class flat_printer : public ::std::unary_function<sheet_item, void>
    {
        const std::string& m_outdir;
    public:
        flat_printer(const std::string& outdir);
        void operator() (const sheet_item& item) const;
    };

    class check_printer : public std::unary_function<sheet_item, void>
    {
        std::ostream& m_os;
    public:
        check_printer(std::ostream& os);
        void operator() (const sheet_item& item) const;
    };

    struct html_printer : public ::std::unary_function<sheet_item, void>
    {
        html_printer(const ::std::string& filepath);
        void operator() (const sheet_item& item) const;
    private:
        const ::std::string& m_filepath;
    };
};

sheet_item::sheet_item(document& doc, const pstring& _name, sheet_t sheet_index, row_t row_size, col_t col_size) :
    name(_name), data(doc, sheet_index, row_size, col_size) {}

sheet_item::flat_printer::flat_printer(const string& outdir) : m_outdir(outdir) {}

void sheet_item::flat_printer::operator() (const sheet_item& item) const
{
    string this_file = m_outdir + '/' + item.name.str() + ".txt";

    ofstream file(this_file.c_str());
    if (!file)
    {
        cerr << "failed to create file: " << this_file << endl;
        return;
    }

    file << "---" << endl;
    file << "Sheet name: " << item.name << endl;
    item.data.dump_flat(file);
}

sheet_item::check_printer::check_printer(std::ostream& os) : m_os(os) {}

void sheet_item::check_printer::operator() (const sheet_item& item) const
{
    item.data.dump_check(m_os, item.name);
}

sheet_item::html_printer::html_printer(const string& filepath) :
    m_filepath(filepath) {}

void sheet_item::html_printer::operator() (const sheet_item& item) const
{
    // file path is expected to be a directory.
    string this_file = m_filepath + '/' + item.name.str() + ".html";
    item.data.dump_html(this_file);
}

class find_sheet_by_name : std::unary_function<sheet_item , bool>
{
    const pstring& m_name;
public:
    find_sheet_by_name(const pstring& name) : m_name(name) {}
    bool operator() (const sheet_item & v) const
    {
        return v.name == m_name;
    }
};

}

struct document_impl
{
    document& m_doc;

    string_pool m_string_pool;
    ixion::model_context m_context;
    date_time_t m_origin_date;
    boost::ptr_vector<sheet_item> m_sheets;
    import_global_settings* mp_settings;
    import_styles* mp_styles;
    import_shared_strings* mp_strings;
    ixion::dirty_formula_cells_t m_dirty_cells;

    document_impl(document& doc) :
        m_doc(doc),
        mp_settings(new import_global_settings(m_doc)),
        mp_styles(new import_styles(m_string_pool)),
        mp_strings(new import_shared_strings(m_string_pool, m_context, *mp_styles))
    {
    }

    ~document_impl()
    {
        delete mp_strings;
        delete mp_styles;
        delete mp_settings;
    }
};

document::document() :
    mp_impl(new document_impl(*this)) {}

document::~document()
{
    delete mp_impl;
}

import_global_settings* document::get_global_settings()
{
    return mp_impl->mp_settings;
}

const import_global_settings* document::get_global_settings() const
{
    return mp_impl->mp_settings;
}

import_shared_strings* document::get_shared_strings()
{
    return mp_impl->mp_strings;
}

const import_shared_strings* document::get_shared_strings() const
{
    return mp_impl->mp_strings;
}

import_styles* document::get_styles()
{
    return mp_impl->mp_styles;
}

const import_styles* document::get_styles() const
{
    return mp_impl->mp_styles;
}

ixion::model_context& document::get_model_context()
{
    return mp_impl->m_context;
}

const ixion::model_context& document::get_model_context() const
{
    return mp_impl->m_context;
}

namespace {

struct sheet_finalizer : unary_function<sheet_item, void>
{
    void operator() (sheet_item& sh)
    {
        sh.data.finalize();
    }
};

}

void document::finalize()
{
    for_each(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), sheet_finalizer());
    calc_formulas();
}

sheet* document::append_sheet(const pstring& sheet_name, row_t row_size, col_t col_size)
{
    pstring sheet_name_safe = mp_impl->m_string_pool.intern(sheet_name).first;
    sheet_t sheet_index = static_cast<sheet_t>(mp_impl->m_sheets.size());

    mp_impl->m_sheets.push_back(
        new sheet_item(
            *this, sheet_name_safe, sheet_index, row_size, col_size));

    mp_impl->m_context.append_sheet(
        sheet_name_safe.get(), sheet_name_safe.size(), row_size, col_size);

    return &mp_impl->m_sheets.back().data;
}

sheet* document::get_sheet(const pstring& sheet_name)
{
    boost::ptr_vector<sheet_item>::iterator it =
        std::find_if(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), find_sheet_by_name(sheet_name));

    if (it == mp_impl->m_sheets.end())
        return NULL;

    return &it->data;
}

sheet* document::get_sheet(sheet_t sheet_pos)
{
    if (static_cast<size_t>(sheet_pos) >= mp_impl->m_sheets.size())
        return NULL;

    return &mp_impl->m_sheets[sheet_pos].data;
}

const sheet* document::get_sheet(sheet_t sheet_pos) const
{
    if (static_cast<size_t>(sheet_pos) >= mp_impl->m_sheets.size())
        return NULL;

    return &mp_impl->m_sheets[sheet_pos].data;
}

void document::calc_formulas()
{
    ixion::iface::model_context& cxt = get_model_context();
    ixion::calculate_cells(cxt, mp_impl->m_dirty_cells, 0);
}

void document::swap(document& other)
{
    std::swap(mp_impl, other.mp_impl);
}

void document::clear()
{
    delete mp_impl;
    mp_impl = new document_impl(*this);
}

void document::dump_flat(const string& outdir) const
{
    cout << "----------------------------------------------------------------------" << endl;
    cout << "  Document content summary" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    mp_impl->mp_strings->dump();

    cout << "number of sheets: " << mp_impl->m_sheets.size() << endl;
    for_each(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), sheet_item::flat_printer(outdir));
}

void document::dump_check(ostream& os) const
{
    for_each(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), sheet_item::check_printer(os));
}

void document::dump_html(const string& outdir) const
{
    for_each(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), sheet_item::html_printer(outdir));
}

sheet_t document::get_sheet_index(const pstring& name) const
{
    boost::ptr_vector<sheet_item>::const_iterator it =
        std::find_if(mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), find_sheet_by_name(name));

    if (it == mp_impl->m_sheets.end())
        return ixion::invalid_sheet;

    boost::ptr_vector<sheet_item>::const_iterator it_beg = mp_impl->m_sheets.begin();
    size_t pos = std::distance(it_beg, it);
    return static_cast<sheet_t>(pos);
}

pstring document::get_sheet_name(sheet_t sheet_pos) const
{
    if (sheet_pos < 0)
        return pstring();

    size_t pos = static_cast<size_t>(sheet_pos);
    if (pos >= mp_impl->m_sheets.size())
        return pstring();

    return mp_impl->m_sheets[pos].name;
}

size_t document::sheet_size() const
{
    return mp_impl->m_sheets.size();
}

void document::set_origin_date(int year, int month, int day)
{
    mp_impl->m_origin_date.year = year;
    mp_impl->m_origin_date.month = month;
    mp_impl->m_origin_date.day = day;
}

void document::insert_dirty_cell(const ixion::abs_address_t& pos)
{
    mp_impl->m_dirty_cells.insert(pos);
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
