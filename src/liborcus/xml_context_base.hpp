/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_XML_CONTEXT_BASE_HPP
#define INCLUDED_ORCUS_XML_CONTEXT_BASE_HPP

#include "xml_stream_handler.hpp"

namespace orcus {

struct session_context;
class tokens;
class xmlns_context;

typedef ::std::pair<xmlns_id_t, xml_token_t> xml_token_pair_t;
typedef ::std::vector<xml_token_pair_t>         xml_elem_stack_t;

class xml_context_base
{
public:
    xml_context_base(const xml_context_base&) = delete;
    xml_context_base& operator=(const xml_context_base&) = delete;

    xml_context_base(session_context& session_cxt, const tokens& tokens);
    virtual ~xml_context_base() = 0;

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const = 0;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name) = 0;
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child) = 0;

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const ::std::vector<xml_token_attr_t>& attrs) = 0;

    /**
     * Called on closing element.
     *
     * @param ns namespace token
     * @param name element name
     *
     * @return true if the base element of the context is closing, false
     *         otherwise.
     */
    virtual bool end_element(xmlns_id_t ns, xml_token_t name) = 0;

    /**
     * Called when passing xml content.  When the content value is transient,
     * the value is not expected to survive beyond the scope of the callback.
     *
     * @param str content value.
     * @param transient whether or not the value is transient.
     */
    virtual void characters(const pstring& str, bool transient) = 0;

    void set_ns_context(const xmlns_context* p);

    void set_config(const config& opt);

    void transfer_common(const xml_context_base& parent);

protected:
    session_context& get_session_context();
    const tokens& get_tokens() const;
    xml_token_pair_t push_stack(xmlns_id_t ns, xml_token_t name);
    bool pop_stack(xmlns_id_t ns, xml_token_t name);
    xml_token_pair_t& get_current_element();
    const xml_token_pair_t& get_current_element() const;
    xml_token_pair_t& get_parent_element();
    const xml_token_pair_t& get_parent_element() const;
    void warn_unhandled() const;
    void warn_unexpected() const;
    void warn(const char* msg) const;

    /**
     * Check if observed element equals expected element.  If not, it throws an
     * xml_structure_error exception.
     *
     * @param elem element observed.
     * @param ns namespace of expected element.
     * @param name name of expected element.
     * @param error custom error message if needed.
     */
    void xml_element_expected(
        const xml_token_pair_t& elem, xmlns_id_t ns, xml_token_t name,
        const ::std::string* error = nullptr);

    void xml_element_expected(
        const xml_token_pair_t& elem, const xml_elem_stack_t& expected_elems);

    const config& get_config() const;

private:
    config m_config;
    const xmlns_context* mp_ns_cxt;
    session_context& m_session_cxt;
    const tokens& m_tokens;
    xml_elem_stack_t m_stack;
};


}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
