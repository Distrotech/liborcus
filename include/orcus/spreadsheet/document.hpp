/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SPREADSHEET_DOCUMENT_HPP__
#define __ORCUS_SPREADSHEET_DOCUMENT_HPP__

#include "orcus/env.hpp"
#include "orcus/interface.hpp"
#include "orcus/spreadsheet/types.hpp"

#include <ostream>

namespace ixion {

class formula_name_resolver;
class model_context;
struct abs_address_t;

}

namespace orcus {

class pstring;

namespace spreadsheet {

class import_global_settings;
class import_shared_strings;
class import_styles;
class sheet;

struct document_impl;

/**
 * Internal document representation used only for testing the filters.  It
 * uses ixion's model_context implementation to store raw cell values.
 */
class ORCUS_DLLPUBLIC document : public orcus::iface::document_dumper
{
    friend class sheet;

    document(const document&); // disabled
    document& operator= (const document&); // disabled

public:
    document();
    ~document();

    import_shared_strings* get_shared_strings();
    const import_shared_strings* get_shared_strings() const;

    import_styles* get_styles();
    const import_styles* get_styles() const;

    sheet* append_sheet(const pstring& sheet_name, row_t row_size, col_t col_size);
    sheet* get_sheet(const pstring& sheet_name);
    sheet* get_sheet(sheet_t sheet_pos);
    const sheet* get_sheet(sheet_t sheet_pos) const;

    void calc_formulas();

    void swap(document& other);

    /**
     * Clear document content, to make it empty.
     */
    void clear();

    /**
     * Dump document content to specified output directory.
     */
    virtual void dump_flat(const std::string& outdir) const;

    /**
     * File name should not contain an extension.  The final name will be
     * [filename] + _ + [sheet name] + .html.
     *
     * @param filename base file name
     */
    virtual void dump_html(const ::std::string& outdir) const;

    /**
     * Dump document content to stdout in the special format used for content
     * verification during unit test.
     */
    virtual void dump_check(std::ostream& os) const;

    sheet_t get_sheet_index(const pstring& name) const;
    pstring get_sheet_name(sheet_t sheet_pos) const;

    size_t sheet_size() const;

    void set_origin_date(int year, int month, int day);
    void set_formula_grammar(formula_grammar_t grammar);
    formula_grammar_t get_formula_grammar() const;

    const ixion::formula_name_resolver* get_formula_name_resolver() const;

    ixion::model_context& get_model_context();
    const ixion::model_context& get_model_context() const;

    void finalize();

private:
    void insert_dirty_cell(const ixion::abs_address_t& pos);

private:
    document_impl* mp_impl;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
