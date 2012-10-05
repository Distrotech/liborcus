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
#include "orcus/gnumeric/gnumeric_sheet_context.hpp"
#include "orcus/gnumeric/gnumeric_cell_context.hpp"
#include "orcus/gnumeric/gnumeric_token_constants.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"


namespace orcus {

gnumeric_sheet_context::gnumeric_sheet_context(
    const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(tokens),
    mp_factory(factory)
{
}

gnumeric_sheet_context::~gnumeric_sheet_context()
{
}

bool gnumeric_sheet_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    if (ns == XMLNS_gnm && name == XML_Cells)
        return false;

    return true;
}

xml_context_base* gnumeric_sheet_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    if (ns == XMLNS_gnm && name == XML_Cells)
        return new gnumeric_cell_context(get_tokens(), mp_factory, mp_sheet);

    return NULL;
}

void gnumeric_sheet_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void gnumeric_sheet_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    push_stack(ns, name);
}

bool gnumeric_sheet_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_gnm)
    {
        switch(name)
        {
            case XML_Name:
            {
                xml_token_pair_t parent = get_parent_element();
                if(parent.first == XMLNS_gnm && parent.second == XML_Sheet)
                    end_table();
                else
                    warn_unhandled();
            }
            break;
            default:
                ;
        }
    }

    return pop_stack(ns, name);
}

void gnumeric_sheet_context::characters(const pstring& str)
{
    chars = str;
}

void gnumeric_sheet_context::end_table()
{
    mp_sheet = mp_factory->append_sheet(chars.get(), chars.size());
}

}