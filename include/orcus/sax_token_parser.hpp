/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_SAX_TOKEN_PARSER_HPP__
#define __ORCUS_SAX_TOKEN_PARSER_HPP__

#include <vector>
#include <algorithm>
#include <functional>

#include "types.hpp"
#include "sax_ns_parser.hpp"

namespace orcus {

namespace sax {

#if ORCUS_DEBUG_SAX_PARSER
template<typename _Attr, typename _Tokens>
class attr_printer : public ::std::unary_function<_Attr, void>
{
public:
    attr_printer(const _Tokens& tokens, const ::std::string& indent) :
        m_tokens(tokens), m_indent(indent) {}

    void operator() (const _Attr& attr) const
    {
        using namespace std;
        cout << m_indent << "  attribute: "
            << attr.ns << ":"
            << m_tokens.get_token_name(attr.name) << "=\""
            << attr.value.str() << "\"" << endl;
    }
private:
    const _Tokens& m_tokens;
    ::std::string m_indent;
};
#endif

}

/**
 * Element properties passed to its handler via start_element() and
 * end_element() calls.
 */
struct sax_token_parser_element
{
    xmlns_id_t ns;
    xml_token_t name;
    pstring raw_name;
    std::vector<xml_token_attr_t> attrs;
};

/**
 * XML parser that tokenizes element and attribute names while parsing.
 */
template<typename _Handler, typename _Tokens>
class sax_token_parser
{
public:
    typedef _Handler    handler_type;
    typedef _Tokens     tokens_map;

    sax_token_parser(const char* content, const size_t size, const tokens_map& tokens, xmlns_context& ns_cxt, handler_type& handler);
    ~sax_token_parser();

    void parse();

private:

    /**
     * Re-route callbacks from the internal sax_parser into the token_parser
     * callbacks.
     */
    class handler_wrapper
    {
        sax_token_parser_element m_elem;
        const tokens_map& m_tokens;
        handler_type& m_handler;

    public:
        handler_wrapper(const tokens_map& tokens, handler_type& handler) :
            m_tokens(tokens), m_handler(handler) {}

        void doctype(const sax::doctype_declaration&) {}

        void start_declaration(const pstring&) {}

        void end_declaration(const pstring&)
        {
            m_elem.attrs.clear();
        }

        void start_element(const sax_ns_parser_element& elem)
        {
            m_elem.ns = elem.ns;
            m_elem.name = tokenize(elem.name);
            m_elem.raw_name = elem.name;
            m_handler.start_element(m_elem);
            m_elem.attrs.clear();
        }

        void end_element(const sax_ns_parser_element& elem)
        {
            m_elem.ns = elem.ns;
            m_elem.name = tokenize(elem.name);
            m_elem.raw_name = elem.name;
            m_handler.end_element(m_elem);
        }

        void characters(const pstring& val, bool transient)
        {
            m_handler.characters(val, transient);
        }

        void attribute(const pstring& /*name*/, const pstring& /*val*/)
        {
            // Right now we don't process XML declaration.
        }

        void attribute(const sax_ns_parser_attribute& attr)
        {
            m_elem.attrs.push_back(
               xml_token_attr_t(
                   attr.ns, tokenize(attr.name), attr.name,
                   attr.value, attr.transient));
        }

    private:

        xml_token_t tokenize(const pstring& name) const
        {
            xml_token_t token = XML_UNKNOWN_TOKEN;
            if (!name.empty())
                token = m_tokens.get_token(name);
            return token;
        }
    };

private:
    xmlns_context& m_ns_cxt;
    handler_wrapper m_wrapper;
    sax_ns_parser<handler_wrapper> m_parser;
};

template<typename _Handler, typename _Tokens>
sax_token_parser<_Handler,_Tokens>::sax_token_parser(
    const char* content, const size_t size, const tokens_map& tokens, xmlns_context& ns_cxt, handler_type& handler) :
    m_ns_cxt(ns_cxt),
    m_wrapper(tokens, handler),
    m_parser(content, size, m_ns_cxt, m_wrapper)
{
}

template<typename _Handler, typename _Tokens>
sax_token_parser<_Handler,_Tokens>::~sax_token_parser()
{
}

template<typename _Handler, typename _Tokens>
void sax_token_parser<_Handler,_Tokens>::parse()
{
    m_parser.parse();
}

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
