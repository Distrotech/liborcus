/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_PARACONTEXT_HPP
#define ORCUS_PARACONTEXT_HPP

#include "xml_context_base.hpp"
#include "odf_styles.hpp"

#include "orcus/pstring.hpp"
#include "orcus/string_pool.hpp"

#include <vector>

namespace orcus {

namespace spreadsheet { namespace iface { class import_shared_strings; }}

/**
 * This class handles <text:p> contexts.
 */
class text_para_context : public xml_context_base
{
public:
    text_para_context(
       session_context& session_cxt, const tokens& tokens,
       spreadsheet::iface::import_shared_strings* ssb, odf_styles_map_type& styles);
    virtual ~text_para_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

    void reset();

    size_t get_string_index() const;
    bool empty() const;

private:
    void flush_segment();

private:
    spreadsheet::iface::import_shared_strings* mp_sstrings;
    odf_styles_map_type& m_styles;

    string_pool m_pool;
    std::vector<pstring> m_span_stack; /// stack of text spans.
    std::vector<pstring> m_contents;
    size_t m_string_index;
    bool m_has_content;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
