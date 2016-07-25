/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_ODF_STYLES_CONTEXT_HPP
#define ORCUS_ODF_STYLES_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "odf_styles.hpp"

#include "orcus/global.hpp"

#include <unordered_map>

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_styles;
}}

/**
 * Context that handles <office:automatic-styles> scope.
 */
class styles_context : public xml_context_base
{
public:
    styles_context(
        session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles, spreadsheet::iface::import_styles* iface_styles);

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);
    virtual void start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void commit_default_styles();

private:
    spreadsheet::iface::import_styles* mp_styles;
    odf_styles_map_type& m_styles;
    std::unique_ptr<xml_context_base> mp_child;

    std::unique_ptr<odf_style> m_current_style;

    // an automatic style corresponds to a cell format and not a real style
    bool m_automatic_styles;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
