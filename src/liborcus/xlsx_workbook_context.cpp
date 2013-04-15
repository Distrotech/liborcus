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

#include "xlsx_workbook_context.hpp"
#include "ooxml_global.hpp"
#include "ooxml_schemas.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"
#include "orcus/global.hpp"

using namespace std;

namespace orcus {

namespace {

class workbook_sheet_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_ooxml_xlsx && attr.name == XML_name)
            m_sheet.name = attr.value.intern();
        else if (attr.ns == NS_ooxml_xlsx && attr.name == XML_sheetId)
        {
            const pstring& val = attr.value;
            if (!val.empty())
                m_sheet.id = strtoul(val.str().c_str(), NULL, 10);
        }
        else if (attr.ns == NS_ooxml_r && attr.name == XML_id)
        {
            m_rid = attr.value.intern();
        }
    }

    const xlsx_rel_sheet_info get_sheet() const { return m_sheet; }
    const pstring& get_rid() const { return m_rid; }

private:
    pstring m_rid;
    xlsx_rel_sheet_info m_sheet;
};

}

xlsx_workbook_context::xlsx_workbook_context(session_context& session_cxt, const tokens& tokens) :
    xml_context_base(session_cxt, tokens) {}

xlsx_workbook_context::~xlsx_workbook_context() {}

bool xlsx_workbook_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_workbook_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_workbook_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_workbook_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_workbook:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);
        }
        break;
        case XML_sheets:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_workbook);
        break;
        case XML_sheet:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_sheets);
            workbook_sheet_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            m_sheets.push_back(new xlsx_rel_sheet_info(func.get_sheet()));
            const xlsx_rel_sheet_info& info = m_sheets.back();
            m_sheet_info.insert(
                opc_rel_extras_t::value_type(func.get_rid(), &info));
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_workbook_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_workbook_context::characters(const pstring& str) {}

void xlsx_workbook_context::pop_sheet_info(opc_rel_extras_t& sheets)
{
    m_sheet_info.swap(sheets);
}

}
