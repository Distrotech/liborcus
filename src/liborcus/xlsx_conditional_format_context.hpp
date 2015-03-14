/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_XLSX_CONDITIONAL_FORMAT_CONTEXT_HPP
#define ORCUS_XLSX_CONDITIONAL_FORMAT_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "ooxml_types.hpp"
#include "xlsx_types.hpp"

#include "orcus/spreadsheet/types.hpp"
#include "orcus/string_pool.hpp"

namespace orcus {

struct session_context;

namespace spreadsheet { namespace iface {
    class import_conditional_format;
} }

struct cfvo_values;

class xlsx_conditional_format_context : public xml_context_base
{
public:
    
    xlsx_conditional_format_context(
            session_context& session_cxt, const tokens& tokens,
            spreadsheet::iface::import_conditional_format& import_cond_format);
    virtual ~xlsx_conditional_format_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    spreadsheet::iface::import_conditional_format& m_cond_format;

    string_pool m_pool;
    pstring m_cur_str;

    std::vector<cfvo_values> m_cfvo_values;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
