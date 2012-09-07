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

#include "orcus/pstring.hpp"
#include "orcus/env.hpp"

#include "sheet.hpp"

#include <ostream>

#include <ixion/model_context.hpp>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>

namespace orcus { namespace spreadsheet {

class import_shared_strings;
class import_styles;

/**
 * Internal document representation used only for testing the filters.  It
 * uses ixion's model_context implementation to store raw cell values.
 */
class ORCUS_DLLPUBLIC document : private ::boost::noncopyable
{
    friend class sheet;

public:
    /**
     * Single sheet entry which consists of a sheet name and a sheet data.
     * Use the printer function object to print sheet content with for_each
     * function.
     */
    struct sheet_item : private ::boost::noncopyable
    {
        pstring name;
        sheet   data;
        sheet_item(document& doc, const pstring& _name, sheet_t sheet);

        struct printer : public ::std::unary_function<sheet_item, void>
        {
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

    document();
    ~document();

    import_shared_strings* get_shared_strings();
    const import_shared_strings* get_shared_strings() const;

    import_styles* get_styles();
    const import_styles* get_styles() const;

    sheet* append_sheet(const pstring& sheet_name);
    sheet* get_sheet(const pstring& sheet_name);

    void calc_formulas();

    /**
     * Dump document content to stdout for debugging.
     */
    void dump() const;

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

    ixion::sheet_t get_sheet_index(const pstring& name) const;
    pstring get_sheet_name(ixion::sheet_t) const;

private:
    ixion::model_context& get_model_context();
    const ixion::model_context& get_model_context() const;
    void insert_dirty_cell(const ixion::abs_address_t& pos);

private:
    ixion::model_context m_context;
    ::boost::ptr_vector<sheet_item> m_sheets;
    import_shared_strings* mp_strings;
    import_styles* mp_styles;
    ixion::dirty_formula_cells_t m_dirty_cells;
};

}}

#endif
