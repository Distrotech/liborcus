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

#ifndef __ORCUS_XLSX_SHEET_CONTEXT_HPP__
#define __ORCUS_XLSX_SHEET_CONTEXT_HPP__

#include "orcus/xml_context.hpp"
#include "orcus/model/types.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {
    class sheet;
}}

/**
 * Top-level context for xl/worksheets/sheet<num>.xml.
 */
class xlsx_sheet_context : public xml_context_base
{
public:
    enum cell_type {
        cell_type_string,
        cell_type_formula_string,
        cell_type_value,
        cell_type_boolean,
        cell_type_error,
        cell_type_inline_string
    };

    xlsx_sheet_context(const tokens& tokens, spreadsheet::iface::sheet* sheet);
    virtual ~xlsx_sheet_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

private:
    void end_element_cell();

private:
    spreadsheet::iface::sheet* mp_sheet; /// sheet model instance for the loaded document.
    spreadsheet::row_t m_cur_row;
    spreadsheet::col_t m_cur_col;
    cell_type    m_cur_cell_type;
    size_t       m_cur_cell_xf;
    pstring      m_cur_str;
    pstring      m_cur_value;
    pstring      m_cur_formula_type;
    pstring      m_cur_formula_ref;
    pstring      m_cur_formula_str;
    int          m_cur_shared_formula_id; /// 0-based shared formula index.
};

}

#endif
