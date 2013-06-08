/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#ifndef __ORCUS_SAX_NS_PARSER_HPP__
#define __ORCUS_SAX_NS_PARSER_HPP__

#include "sax_parser.hpp"
#include "xml_namespace.hpp"

#include <boost/unordered_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

struct sax_ns_parser_element
{
    xmlns_id_t ns;         // element namespace
    pstring ns_alias;      // element namespace alias
    pstring name;          // element name
    const char* begin_pos; // position of the opening brace '<'.
    const char* end_pos;   // position of the char after the closing brace '>'.
};

struct sax_ns_parser_attribute
{
    xmlns_id_t ns;    // attribute namespace
    pstring ns_alias; // attribute namespace alias
    pstring name;     // attribute name
    pstring value;    // attribute value
};

namespace __sax {

struct entity_name
{
    pstring ns;
    pstring name;

    entity_name(const pstring& _ns, const pstring& _name) :
        ns(_ns), name(_name) {}

    bool operator== (const entity_name& other) const
    {
        return other.ns == ns && other.name == name;
    }

    struct hash
    {
        size_t operator() (const entity_name& v) const
        {
            static pstring::hash hasher;
            return hasher(v.ns) + hasher(v.name);
        }
    };
};

typedef boost::unordered_set<pstring, pstring::hash> ns_keys_type;
typedef boost::unordered_set<entity_name, entity_name::hash> entity_names_type;

struct elem_scope
{
    xmlns_id_t ns;
    pstring name;
    ns_keys_type ns_keys;
};

typedef boost::ptr_vector<elem_scope> elem_scopes_type;

class pop_ns_by_key : std::unary_function<pstring, void>
{
    xmlns_context& m_cxt;
public:
    pop_ns_by_key(xmlns_context& cxt) : m_cxt(cxt) {}
    void operator() (const pstring& key)
    {
        m_cxt.pop(key);
    }
};

}

/**
 * SAX based XML parser with proper namespace handling.
 */
template<typename _Handler>
class sax_ns_parser
{
public:
    typedef _Handler handler_type;

    sax_ns_parser(const char* content, const size_t size, xmlns_context& ns_cxt, handler_type& handler);
    ~sax_ns_parser();

    void parse();

private:
    /**
     * Re-route callbacks from the internal sax_parser into sax_ns_parser
     * callbacks.
     */
    class handler_wrapper
    {
        __sax::elem_scopes_type m_scopes;
        __sax::ns_keys_type m_ns_keys;
        __sax::entity_names_type m_attrs;

        sax_ns_parser_element m_elem;
        sax_ns_parser_attribute m_attr;

        xmlns_context& m_ns_cxt;
        handler_type& m_handler;

        bool m_declaration;

    public:
        handler_wrapper(xmlns_context& ns_cxt, handler_type& handler) : m_ns_cxt(ns_cxt), m_handler(handler), m_declaration(false) {}

        void start_declaration(const pstring& name)
        {
            m_declaration = true;
            m_handler.start_declaration(name);
        }

        void end_declaration(const pstring& name)
        {
            m_declaration = false;
            m_handler.end_declaration(name);
        }

        void start_element(const sax_parser_element& elem)
        {
            m_scopes.push_back(new __sax::elem_scope);
            __sax::elem_scope& scope = m_scopes.back();
            scope.ns = m_ns_cxt.get(elem.ns);
            scope.name = elem.name;
            scope.ns_keys.swap(m_ns_keys);

            m_elem.ns = scope.ns;
            m_elem.ns_alias = elem.ns;
            m_elem.name = scope.name;
            m_elem.begin_pos = elem.begin_pos;
            m_elem.end_pos = elem.end_pos;
            m_handler.start_element(m_elem);

            m_attrs.clear();
        }

        void end_element(const sax_parser_element& elem)
        {
            __sax::elem_scope& scope = m_scopes.back();
            if (scope.ns != m_ns_cxt.get(elem.ns) || scope.name != elem.name)
                throw malformed_xml_error("mis-matching closing element.");

            m_elem.ns = scope.ns;
            m_elem.ns_alias = elem.ns;
            m_elem.name = scope.name;
            m_elem.begin_pos = elem.begin_pos;
            m_elem.end_pos = elem.end_pos;
            m_handler.end_element(m_elem);

            // Pop all namespaces declared in this scope.
            std::for_each(scope.ns_keys.begin(), scope.ns_keys.end(), __sax::pop_ns_by_key(m_ns_cxt));

            m_scopes.pop_back();
        }

        void characters(const pstring& val)
        {
            m_handler.characters(val);
        }

        void attribute(const pstring& ns, const pstring& name, const pstring& val)
        {
            if (m_declaration)
            {
                // XML declaration attribute.  Pass it through to the handler without namespace.
                m_handler.attribute(name, val);
                return;
            }

            if (m_attrs.count(__sax::entity_name(ns, name)) > 0)
                throw malformed_xml_error("You can't define two attributes of the same name in the same element.");

            m_attrs.insert(__sax::entity_name(ns, name));

            if (ns.empty() && name == "xmlns")
            {
                // Default namespace
                m_ns_cxt.push(pstring(), val);
                m_ns_keys.insert(pstring());
                return;
            }

            if (ns == "xmlns")
            {
                // Namespace alias
                if (!name.empty())
                {
                    m_ns_cxt.push(name, val);
                    m_ns_keys.insert(name);
                }
                return;
            }

            m_attr.ns = m_ns_cxt.get(ns);
            m_attr.ns_alias = ns;
            m_attr.name = name;
            m_attr.value = val;
            m_handler.attribute(m_attr);
        }
    };

private:
    handler_wrapper m_wrapper;
    sax_parser<handler_wrapper> m_parser;
};

template<typename _Handler>
sax_ns_parser<_Handler>::sax_ns_parser(
    const char* content, const size_t size, xmlns_context& ns_cxt, handler_type& handler) :
    m_wrapper(ns_cxt, handler), m_parser(content, size, m_wrapper)
{
}

template<typename _Handler>
sax_ns_parser<_Handler>::~sax_ns_parser()
{
}

template<typename _Handler>
void sax_ns_parser<_Handler>::parse()
{
    m_parser.parse();
}

}

#endif
