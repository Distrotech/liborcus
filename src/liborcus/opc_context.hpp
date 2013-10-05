/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_OPC_CONTEXT_HPP__
#define __ORCUS_OPC_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include "ooxml_types.hpp"

#include <boost/unordered_set.hpp>
#include <vector>

namespace orcus {

/**
 * Main context class for the [Content_Types].xml part.  This context does
 * not use any child contexts; [Content_Types].xml part is simple enough
 * that we can handle all in a single context class.
 */
class opc_content_types_context : public xml_context_base
{
public:
    typedef boost::unordered_set<pstring, pstring::hash> ct_cache_type;

    opc_content_types_context(session_context& session_cxt, const tokens& _tokens);
    virtual ~opc_content_types_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base *child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t> &attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring &str, bool transient);

    /**
     * Swap stored xml part info with the instance passed as the argument.
     * Calling this will clear the storage.
     *
     * @param parts instance to swap the stored xml part info with.
     */
    void pop_parts(::std::vector<xml_part_t>& parts);

    /**
     * Swap stored xml extension info with the instance passed as the
     * argument. Calling this will clear the storage.
     *
     * @param parts instance to swap the stored extension info with.
     */
    void pop_ext_defaults(::std::vector<xml_part_t>& ext_defaults);

private:
    ct_cache_type m_ct_cache; // content type cache;
    ::std::vector<xml_part_t> m_parts;
    ::std::vector<xml_part_t> m_ext_defaults;
};

/**
 * Context class for relations parts.
 */
class opc_relations_context : public xml_context_base
{
public:
    typedef boost::unordered_set<pstring, pstring::hash> schema_cache_type;

    opc_relations_context(session_context& session_cxt, const tokens& _tokens);
    virtual ~opc_relations_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base *child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t> &attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring &str, bool transient);

    void init();
    void pop_rels(::std::vector<opc_rel_t>& rels);

private:
    schema_cache_type m_schema_cache;
    ::std::vector<opc_rel_t> m_rels;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
