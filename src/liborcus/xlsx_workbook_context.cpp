/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_workbook_context.hpp"
#include "ooxml_global.hpp"
#include "ooxml_schemas.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"
#include "session_context.hpp"

#include "orcus/global.hpp"
#include "orcus/measurement.hpp"

using namespace std;

namespace orcus {

namespace {

class workbook_sheet_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    workbook_sheet_attr_parser(session_context* cxt) : m_cxt(cxt) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_ooxml_xlsx && attr.name == XML_name)
            m_sheet.name = m_cxt->m_string_pool.intern(attr.value).first;
        else if (attr.ns == NS_ooxml_xlsx && attr.name == XML_sheetId)
        {
            const pstring& val = attr.value;
            if (!val.empty())
                m_sheet.id = to_long(val);
        }
        else if (attr.ns == NS_ooxml_r && attr.name == XML_id)
        {
            m_rid = m_cxt->m_string_pool.intern(attr.value).first;
        }
    }

    const xlsx_rel_sheet_info get_sheet() const { return m_sheet; }
    const pstring& get_rid() const { return m_rid; }

private:
    session_context* m_cxt;
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
            if (get_config().debug)
                print_attrs(get_tokens(), attrs);
        }
        break;
        case XML_sheets:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_workbook);
        break;
        case XML_sheet:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_sheets);
            workbook_sheet_attr_parser func(&get_session_context());
            func = for_each(attrs.begin(), attrs.end(), func);
            m_sheet_info.data.insert(
                opc_rel_extras_t::map_type::value_type(
                    func.get_rid(), new xlsx_rel_sheet_info(func.get_sheet())));
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

void xlsx_workbook_context::characters(const pstring& str, bool transient) {}

void xlsx_workbook_context::pop_sheet_info(opc_rel_extras_t& sheets)
{
    m_sheet_info.swap(sheets);
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
