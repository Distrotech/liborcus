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

#ifndef __ORCUS_GNUMERIC_SHEET_CONTEXT_HPP__
#define __ORCUS_GNUMERIC_SHEET_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include "orcus/spreadsheet/types.hpp"

#include <boost/scoped_ptr.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

struct gnumeric_style_region
{
    spreadsheet::row_t start_row;
    spreadsheet::row_t end_row;
    spreadsheet::col_t start_col;
    spreadsheet::col_t end_col;

    size_t xf_id;
};

class gnumeric_sheet_context : public xml_context_base
{
public:
    gnumeric_sheet_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~gnumeric_sheet_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

private:
    void start_style_region(const xml_attrs_t& attrs);
    void start_style(const xml_attrs_t& attrs);
    void start_font(const xml_attrs_t& attrs);

    void end_table();
    void end_style();
    void end_font();
    void end_style_region();

private:
    spreadsheet::iface::import_factory* mp_factory;

    spreadsheet::iface::import_sheet* mp_sheet;

    boost::scoped_ptr<gnumeric_style_region> mp_region_data;

    /**
    * Used for temporary storage of characters
    */
    pstring chars;
};

} // namespace orcus

#endif
