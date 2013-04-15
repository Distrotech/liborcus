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

#ifndef __ORCUS_GNUMERIC_CELL_CONTEXT_HPP__
#define __ORCUS_GNUMERIC_CELL_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include <boost/scoped_ptr.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

struct gnumeric_cell_data;

class gnumeric_cell_context : public xml_context_base
{
public:
    gnumeric_cell_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory, spreadsheet::iface::import_sheet* sheet);
    virtual ~gnumeric_cell_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str);

private:
    void start_cell(const xml_attrs_t& attrs);
    void end_cell();
private:
    spreadsheet::iface::import_factory* mp_factory;

    boost::scoped_ptr<gnumeric_cell_data> mp_cell_data;

    /**
    * Used for temporary storage of characters
    */
    pstring chars;

    spreadsheet::iface::import_sheet* mp_sheet;
};

} // namespace orcus

#endif
