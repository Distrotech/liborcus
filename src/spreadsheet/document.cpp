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
#include "orcus/spreadsheet/auto_filter.hpp"

#include "orcus/pstring.hpp"
#include "orcus/types.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/global.hpp"

#include <ixion/formula.hpp>
#include <ixion/formula_result.hpp>
#include <ixion/matrix.hpp>
#include <ixion/model_context.hpp>
#include <ixion/formula_name_resolver.hpp>
#include <ixion/interface/table_handler.hpp>

#include <iostream>
#include <fstream>
#include <map>

using namespace std;

namespace orcus { namespace spreadsheet {

namespace {

/**
 * Single sheet entry which consists of a sheet name and a sheet data.
 * Use the printer function object to print sheet content with for_each
 * function.
 */
struct sheet_item
{
    sheet_item(const sheet_item&) = delete;
    sheet_item& operator=(const sheet_item&) = delete;

    pstring name;
    sheet   data;
    sheet_item(document& doc, const pstring& _name, sheet_t sheet_index, row_t row_size, col_t col_size);

    class flat_printer : public ::std::unary_function<std::unique_ptr<sheet_item>, void>
    {
        const std::string& m_outdir;
    public:
        flat_printer(const std::string& outdir);
        void operator() (const std::unique_ptr<sheet_item>& item) const;
    };

    class check_printer : public std::unary_function<std::unique_ptr<sheet_item>, void>
    {
        std::ostream& m_os;
    public:
        check_printer(std::ostream& os);
        void operator() (const std::unique_ptr<sheet_item>& item) const;
    };

    struct html_printer : public ::std::unary_function<std::unique_ptr<sheet_item>, void>
    {
        html_printer(const ::std::string& filepath);
        void operator() (const std::unique_ptr<sheet_item>& item) const;
    private:
        const ::std::string& m_filepath;
    };
};

typedef std::map<pstring, std::unique_ptr<table_t>> table_store_type;

sheet_item::sheet_item(document& doc, const pstring& _name, sheet_t sheet_index, row_t row_size, col_t col_size) :
    name(_name), data(doc, sheet_index, row_size, col_size) {}

sheet_item::flat_printer::flat_printer(const string& outdir) : m_outdir(outdir) {}

void sheet_item::flat_printer::operator() (const std::unique_ptr<sheet_item>& item) const
{
    string this_file = m_outdir + '/' + item->name.str() + ".txt";

    ofstream file(this_file.c_str());
    if (!file)
    {
        cerr << "failed to create file: " << this_file << endl;
        return;
    }

    file << "---" << endl;
    file << "Sheet name: " << item->name << endl;
    item->data.dump_flat(file);
}

sheet_item::check_printer::check_printer(std::ostream& os) : m_os(os) {}

void sheet_item::check_printer::operator() (const std::unique_ptr<sheet_item>& item) const
{
    item->data.dump_check(m_os, item->name);
}

sheet_item::html_printer::html_printer(const string& filepath) :
    m_filepath(filepath) {}

void sheet_item::html_printer::operator() (const std::unique_ptr<sheet_item>& item) const
{
    // file path is expected to be a directory.
    string this_file = m_filepath + '/' + item->name.str() + ".html";
    item->data.dump_html(this_file);
}

class find_sheet_by_name : std::unary_function<std::unique_ptr<sheet_item> , bool>
{
    const pstring& m_name;
public:
    find_sheet_by_name(const pstring& name) : m_name(name) {}
    bool operator() (const std::unique_ptr<sheet_item>& v) const
    {
        return v->name == m_name;
    }
};

class find_column_by_name : std::unary_function<table_column_t, bool>
{
    const pstring& m_name;
public:
    find_column_by_name(const pstring& name) : m_name(name) {}

    bool operator() (const table_column_t& col) const
    {
        return col.name == m_name;
    }
};

void adjust_row_range(ixion::abs_range_t& range, const table_t& tab, ixion::table_areas_t areas)
{
    bool headers = (areas & ixion::table_area_headers);
    bool data    = (areas & ixion::table_area_data);
    bool totals  = (areas & ixion::table_area_totals);

    if (headers)
    {
        if (data)
        {
            if (totals)
            {
                // All areas.
                return;
            }

            // Headers + data
            range.last.row -= tab.totals_row_count;
            return;
        }

        if (totals)
        {
            // Header + total is invalid.
            range = ixion::abs_range_t(ixion::abs_range_t::invalid);
            return;
        }

        // Headers only.
        range.last.row = range.first.row;
        return;
    }

    if (data)
    {
        ++range.first.row;

        if (totals)
        {
            // Data + total
            return;
        }

        // Data only
        range.last.row -= tab.totals_row_count;
        return;
    }

    if (totals)
    {
        // Total only
        if (!tab.totals_row_count)
        {
            // This table has not total rows.  Return empty range.
            range = ixion::abs_range_t();
            return;
        }

        range.first.row = range.last.row - tab.totals_row_count - 1;
        return;
    }

    // Empty range.
    range = ixion::abs_range_t();
}

class table_handler : public ixion::iface::table_handler
{
    const ixion::model_context& m_context;
    const table_store_type& m_tables;

    const table_t* find_table(const ixion::abs_address_t& pos) const
    {
        auto it = m_tables.begin(), it_end = m_tables.end();
        for (; it != it_end; ++it)
        {
            const table_t* p = it->second.get();
            if (p->range.contains(pos))
                return p;
        }

        return NULL;
    }

    pstring get_string(ixion::string_id_t sid) const
    {
        if (sid == ixion::empty_string_id)
            return pstring();

        const std::string* p = m_context.get_string(sid);
        if (!p || p->empty())
            return pstring();

        return pstring(&(*p)[0], p->size());
    }

    col_t find_column(const table_t& tab, const pstring& name, size_t offset) const
    {
        if (offset >= tab.columns.size())
            return -1;

        table_t::columns_type::const_iterator it_beg = tab.columns.begin();
        table_t::columns_type::const_iterator it_end = tab.columns.end();

        std::advance(it_beg, offset);
        table_t::columns_type::const_iterator it =
            std::find_if(it_beg, it_end, find_column_by_name(name));

        if (it == it_end)
            // not found.
            return -1;

        size_t dist = std::distance(tab.columns.begin(), it);
        return tab.range.first.column + dist;
    }

    ixion::abs_range_t get_range_from_table(
        const table_t& tab, ixion::string_id_t column_first, ixion::string_id_t column_last,
        ixion::table_areas_t areas) const
    {
        if (column_first != ixion::empty_string_id)
        {
            pstring col1_name = get_string(column_first);
            if (col1_name.empty())
                return ixion::abs_range_t(ixion::abs_range_t::invalid);

            col_t col1_index = find_column(tab, col1_name, 0);
            if (column_last != ixion::empty_string_id)
            {
                pstring col2_name = get_string(column_last);

                // column range table reference.
                col_t col2_index = find_column(tab, col2_name, col1_index);
                ixion::abs_range_t range = tab.range;
                range.first.column = col1_index;
                range.last.column = col2_index;
                adjust_row_range(range, tab, areas);
                return range;
            }

            // single column table reference.
            ixion::abs_range_t range = tab.range;
            range.first.column = range.last.column = col1_index;
            adjust_row_range(range, tab, areas);
            return range;
        }

        return ixion::abs_range_t();
    }

public:
    table_handler(const ixion::model_context& cxt, const table_store_type& tables) :
        m_context(cxt), m_tables(tables) {}

    virtual ixion::abs_range_t get_range(
        const ixion::abs_address_t& pos, ixion::string_id_t column_first, ixion::string_id_t column_last,
        ixion::table_areas_t areas) const
    {
        const table_t* tab = find_table(pos);
        if (!tab)
            return ixion::abs_range_t(ixion::abs_range_t::invalid);

        return get_range_from_table(*tab, column_first, column_last, areas);

    }

    virtual ixion::abs_range_t get_range(
        ixion::string_id_t table, ixion::string_id_t column_first, ixion::string_id_t column_last,
        ixion::table_areas_t areas) const
    {
        pstring tab_name = get_string(table);
        if (tab_name.empty())
            // no table name given.
            return ixion::abs_range_t(ixion::abs_range_t::invalid);

        auto it = m_tables.find(tab_name);
        if (it == m_tables.end())
            // no table by this name found.
            return ixion::abs_range_t(ixion::abs_range_t::invalid);

        const table_t* tab = it->second.get();
        return get_range_from_table(*tab, column_first, column_last, areas);
    }
};

typedef std::vector<std::unique_ptr<sheet_item>> sheet_items_type;

}

struct document_impl
{
    document_impl(const document_impl&) = delete;
    document_impl& operator=(const document_impl&) = delete;

    document& m_doc;

    string_pool m_string_pool;
    ixion::model_context m_context;
    date_time_t m_origin_date;
    sheet_items_type m_sheets;
    import_styles* mp_styles;
    import_shared_strings* mp_strings;
    ixion::dirty_formula_cells_t m_dirty_cells;

    std::unique_ptr<ixion::formula_name_resolver> mp_name_resolver;
    formula_grammar_t m_grammar;

    table_store_type m_tables;
    table_handler m_table_handler;

    document_impl(document& doc) :
        m_doc(doc),
        mp_styles(new import_styles(m_string_pool)),
        mp_strings(new import_shared_strings(m_string_pool, m_context, *mp_styles)),
        mp_name_resolver(ixion::formula_name_resolver::get(ixion::formula_name_resolver_t::excel_a1, &m_context)),
        m_grammar(formula_grammar_xlsx_2007),
        m_table_handler(m_context, m_tables)
    {
        m_context.set_table_handler(&m_table_handler);
    }

    ~document_impl()
    {
        delete mp_strings;
        delete mp_styles;
    }
};

document::document() :
    mp_impl(new document_impl(*this)) {}

document::~document()
{
    delete mp_impl;
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

string_pool& document::get_string_pool()
{
    return mp_impl->m_string_pool;
}

void document::insert_table(table_t* p)
{
    if (!p)
        return;

    pstring name = p->name;
    mp_impl->m_tables.insert(
        table_store_type::value_type(name, std::unique_ptr<table_t>(p)));
}

const table_t* document::get_table(const pstring& name) const
{
    auto it = mp_impl->m_tables.find(name);
    return it == mp_impl->m_tables.end() ? NULL : it->second.get();
}

namespace {

struct sheet_finalizer : std::unary_function<std::unique_ptr<sheet_item>, void>
{
    void operator() (std::unique_ptr<sheet_item>& sh)
    {
        sh->data.finalize();
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
        make_unique<sheet_item>(
            *this, sheet_name_safe, sheet_index, row_size, col_size));

    mp_impl->m_context.append_sheet(
        sheet_name_safe.get(), sheet_name_safe.size(), row_size, col_size);

    return &mp_impl->m_sheets.back()->data;
}

sheet* document::get_sheet(const pstring& sheet_name)
{
    auto it = std::find_if(
        mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), find_sheet_by_name(sheet_name));

    if (it == mp_impl->m_sheets.end())
        return NULL;

    return &(*it)->data;
}

sheet* document::get_sheet(sheet_t sheet_pos)
{
    if (static_cast<size_t>(sheet_pos) >= mp_impl->m_sheets.size())
        return NULL;

    return &mp_impl->m_sheets[sheet_pos]->data;
}

const sheet* document::get_sheet(sheet_t sheet_pos) const
{
    if (static_cast<size_t>(sheet_pos) >= mp_impl->m_sheets.size())
        return NULL;

    return &mp_impl->m_sheets[sheet_pos]->data;
}

void document::calc_formulas()
{
    ixion::model_context& cxt = get_model_context();
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
    auto it = std::find_if(
        mp_impl->m_sheets.begin(), mp_impl->m_sheets.end(), find_sheet_by_name(name));

    if (it == mp_impl->m_sheets.end())
        return ixion::invalid_sheet;

    auto it_beg = mp_impl->m_sheets.begin();
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

    return mp_impl->m_sheets[pos]->name;
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

void document::set_formula_grammar(formula_grammar_t grammar)
{
    if (mp_impl->m_grammar == grammar)
        return;

    mp_impl->m_grammar = grammar;
    switch (mp_impl->m_grammar)
    {
        case formula_grammar_xlsx_2007:
        case formula_grammar_xlsx_2010:
            mp_impl->mp_name_resolver =
                ixion::formula_name_resolver::get(
                    ixion::formula_name_resolver_t::excel_a1, &mp_impl->m_context);
        break;
        case formula_grammar_ods:
            mp_impl->mp_name_resolver =
                ixion::formula_name_resolver::get(
                    ixion::formula_name_resolver_t::odff, &mp_impl->m_context);
        break;
        case formula_grammar_gnumeric:
            // TODO : Use Excel A1 name resolver for now.
            mp_impl->mp_name_resolver =
                ixion::formula_name_resolver::get(
                    ixion::formula_name_resolver_t::excel_a1, &mp_impl->m_context);
        break;
        default:
            mp_impl->mp_name_resolver.reset();
    }
}

formula_grammar_t document::get_formula_grammar() const
{
    return mp_impl->m_grammar;
}

const ixion::formula_name_resolver* document::get_formula_name_resolver() const
{
    return mp_impl->mp_name_resolver.get();
}

void document::insert_dirty_cell(const ixion::abs_address_t& pos)
{
    mp_impl->m_dirty_cells.insert(pos);
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
