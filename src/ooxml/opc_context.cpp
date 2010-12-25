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

#include "ooxml/opc_context.hpp"
#include "ooxml/opc_token_constants.hpp"
#include "ooxml/content_types.hpp"
#include "ooxml/schemas.hpp"
#include "global.hpp"

#include <iostream>
#include <algorithm>

using namespace std;

namespace orcus {

namespace {

class types_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    explicit types_attr_parser() : 
        m_default_ns(XMLNS_UNKNOWN_TOKEN) {}

    types_attr_parser(const types_attr_parser& r) :
        m_default_ns(r.m_default_ns) {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_xmlns)
        {
            if (attr.value != SCH_opc_content_types)
                throw xml_structure_error("invalid namespace for types element!");
            m_default_ns = XMLNS_ct;
        }
    }

    xmlns_token_t get_default_ns() const
    {
        return m_default_ns;
    }
private:
    xmlns_token_t m_default_ns;
};

class part_ext_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    part_ext_attr_parser(
        opc_content_types_context::ct_cache_type* p_ct_cache, xml_token_t attr_name) : 
        mp_ct_cache(p_ct_cache),
        m_attr_name(attr_name),
        m_content_type(NULL) {}

    part_ext_attr_parser(const part_ext_attr_parser& r) :
        mp_ct_cache(r.mp_ct_cache),
        m_attr_name(r.m_attr_name),
        m_name(r.m_name), 
        m_content_type(r.m_content_type) {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.name == m_attr_name)
            m_name = attr.value;
        else if (attr.name == XML_ContentType)
            m_content_type = to_content_type(attr.value);
    }

    const pstring& get_name() const { return m_name; }
    const char* get_content_type() const { return m_content_type; }

private:
    content_type_t to_content_type(const pstring& p) const
    {
        opc_content_types_context::ct_cache_type::const_iterator itr = 
            mp_ct_cache->find(p);
        if (itr == mp_ct_cache->end())
        {
            cout << "unknown content type: " << p << endl;
            return NULL;
        }
        const pstring& val = *itr;
        return val.get();
    }

private:
    const opc_content_types_context::ct_cache_type* mp_ct_cache;
    xml_token_t m_attr_name;
    pstring m_name;
    content_type_t m_content_type;
};

}

opc_content_types_context::opc_content_types_context(const tokens& _tokens) :
    xml_context_base(_tokens)
{
    // build content type cache.
    for (content_type_t* p = CT_all; *p; ++p)
        m_ct_cache.insert(pstring(*p));
}

opc_content_types_context::~opc_content_types_context()
{
}

bool opc_content_types_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* opc_content_types_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void opc_content_types_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child)
{
}

void opc_content_types_context::start_element(xmlns_token_t ns, xml_token_t name, const::std::vector<xml_attr_t> &attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_Types:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);

            print_attrs(get_tokens(), attrs);

            xmlns_token_t default_ns = 
                for_each(attrs.begin(), attrs.end(), types_attr_parser()).get_default_ns();

            // the namespace for Types element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_Override:
        {
            xml_element_expected(parent, XMLNS_ct, XML_Types);
            part_ext_attr_parser func(&m_ct_cache, XML_PartName);
            func = for_each(attrs.begin(), attrs.end(), func);

            // We need to use allocated strings for part names here because
            // the part names need to survive after the [Content_Types].xml
            // stream is destroyed.
            m_parts.push_back(
                xml_part_t(func.get_name().str(), func.get_content_type()));
        }
        break;
        case XML_Default:
        {
            xml_element_expected(parent, XMLNS_ct, XML_Types);
            part_ext_attr_parser func(&m_ct_cache, XML_Extension);
            func = for_each(attrs.begin(), attrs.end(), func);

            // Like the part names, we need to use allocated strings for 
            // extension names.
            m_ext_defaults.push_back(
                xml_part_t(func.get_name().str(), func.get_content_type()));
        }
        break;
        default:
            warn_unhandled();
    }
}

bool opc_content_types_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void opc_content_types_context::characters(const pstring &str)
{
}

void opc_content_types_context::pop_parts(vector<xml_part_t>& parts)
{
    m_parts.swap(parts);
}

void opc_content_types_context::pop_ext_defaults(vector<xml_part_t>& ext_defaults)
{
    m_ext_defaults.swap(ext_defaults);
}

// ============================================================================

namespace {

/**
 * Attribute parser for Relationships element.
 */
class rels_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    explicit rels_attr_parser() : 
        m_default_ns(XMLNS_UNKNOWN_TOKEN) {}

    rels_attr_parser(const rels_attr_parser& r) :
        m_default_ns(r.m_default_ns) {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_xmlns)
        {
            if (attr.value != SCH_opc_relationships)
                throw xml_structure_error("invalid namespace for types element!");
            m_default_ns = XMLNS_rel;
        }
    }

    xmlns_token_t get_default_ns() const
    {
        return m_default_ns;
    }
private:
    xmlns_token_t m_default_ns;
};

}

opc_relations_context::opc_relations_context(const tokens &_tokens) :
    xml_context_base(_tokens)
{
}

opc_relations_context::~opc_relations_context()
{
}

bool opc_relations_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* opc_relations_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void opc_relations_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base *child)
{
}

void opc_relations_context::start_element(xmlns_token_t ns, xml_token_t name, const vector<xml_attr_t> &attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_Relationships:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);

            xmlns_token_t default_ns =
                for_each(attrs.begin(), attrs.end(), rels_attr_parser()).get_default_ns();

            // the namespace for Types element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_Relationship:
        {
            xml_element_expected(parent, XMLNS_rel, XML_Relationships);
            print_attrs(get_tokens(), attrs);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool opc_relations_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void opc_relations_context::characters(const pstring &str)
{
}

}
