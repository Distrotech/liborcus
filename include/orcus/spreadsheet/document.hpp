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

#ifndef __ORCUS_SPREADSHEET_DOCUMENT_HPP__
#define __ORCUS_SPREADSHEET_DOCUMENT_HPP__

#include "orcus/env.hpp"
#include "orcus/spreadsheet/types.hpp"

#include <ostream>

namespace ixion {
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
class ORCUS_DLLPUBLIC document
{
    friend class sheet;

    document(const document&); // disabled
    document& operator= (const document&); // disabled

public:
    document();
    ~document();

    import_global_settings* get_global_settings();
    const import_global_settings* get_global_settings() const;

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
    void dump_flat(const std::string& outdir) const;

    /**
     * Dump document content to stdout in the special format used for content
     * verification during unit test.
     */
    void dump_check(std::ostream& os) const;

    /**
     * File name should not contain an extension.  The final name will be
     * [filename] + _ + [sheet name] + .html.
     *
     * @param filename base file name
     */
    void dump_html(const ::std::string& filename) const;

    sheet_t get_sheet_index(const pstring& name) const;
    pstring get_sheet_name(sheet_t sheet_pos) const;

    size_t sheet_size() const;

    void set_origin_date(int year, int month, int day);

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
