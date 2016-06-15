/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ODF_NUMBER_FORMATTING_CONTEXT_HPP
#define ODF_NUMBER_FORMATTING_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "odf_styles.hpp"
#include "odf_styles_context.hpp"
#include "orcus/global.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_styles;
}}


/**
 * Context that handles <number:xyz> scope.
 */
class number_formatting_context : public xml_context_base
{
public:
    number_formatting_context(
        session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles, spreadsheet::iface::import_styles* iface_styles, number_formatting_style*);

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);
    virtual void start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:

    spreadsheet::iface::import_styles* mp_styles;
    odf_styles_map_type& m_styles;
    number_formatting_style* m_current_style;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
