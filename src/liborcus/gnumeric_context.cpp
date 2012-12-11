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

#include "gnumeric_context.hpp"
#include "gnumeric_token_constants.hpp"
#include "gnumeric_sheet_context.hpp"
#include "orcus/global.hpp"
#include "orcus/spreadsheet/import_interface.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

}

// ============================================================================

gnumeric_content_xml_context::gnumeric_content_xml_context(const tokens& tokens, spreadsheet::iface::import_factory* factory) :
    xml_context_base(tokens),
    mp_factory(factory)
{
}

gnumeric_content_xml_context::~gnumeric_content_xml_context()
{
}

bool gnumeric_content_xml_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    if (ns == XMLNS_gnm && name == XML_Sheet)
        return false;

    return true;
}

xml_context_base* gnumeric_content_xml_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    if (ns == XMLNS_gnm && name == XML_Sheet)
        return new gnumeric_sheet_context(get_tokens(), mp_factory);

    return NULL;
}

void gnumeric_content_xml_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void gnumeric_content_xml_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    push_stack(ns, name);

    if (ns == XMLNS_gnm)
    {
        switch (name)
        {
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool gnumeric_content_xml_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_gnm)
    {
        switch (name)
        {

        }
    }
    return pop_stack(ns, name);
}

void gnumeric_content_xml_context::characters(const pstring& str)
{
}

}
