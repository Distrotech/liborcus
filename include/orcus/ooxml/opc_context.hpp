/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#ifndef __ORCUS_OPC_CONTEXT_HPP__
#define __ORCUS_OPC_CONTEXT_HPP__

#include "xml_context_base.hpp"
#include "orcus/ooxml/ooxml_types.hpp"

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

    opc_content_types_context(const tokens& _tokens);
    virtual ~opc_content_types_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_token_attr_t> &attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring &str);

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

    opc_relations_context(const tokens& _tokens);
    virtual ~opc_relations_context();

    virtual bool can_handle_element(xmlns_token_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_token_t ns, xml_token_t name) const;
    virtual void end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child);

    virtual void start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_token_attr_t> &attrs);
    virtual bool end_element(xmlns_token_t ns, xml_token_t name);
    virtual void characters(const pstring &str);

    void init();
    void pop_rels(::std::vector<opc_rel_t>& rels);

private:
    schema_cache_type m_schema_cache;
    ::std::vector<opc_rel_t> m_rels;
};

}

#endif
