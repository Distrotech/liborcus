/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SAX_TOKEN_PARSER_HPP
#define INCLUDED_ORCUS_SAX_TOKEN_PARSER_HPP

#include <vector>
#include <algorithm>
#include <functional>

#include "types.hpp"
#include "sax_ns_parser.hpp"

namespace orcus {

class tokens;

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

class ORCUS_PSR_DLLPUBLIC sax_token_handler_wrapper_base
{
protected:
    xml_token_element_t m_elem;
    const tokens& m_tokens;

    xml_token_t tokenize(const pstring& name) const;
    void set_element(const sax_ns_parser_element& elem);

public:
    sax_token_handler_wrapper_base(const tokens& _tokens);

    void attribute(const pstring& name, const pstring& val);
    void attribute(const sax_ns_parser_attribute& attr);
};

/**
 * XML parser that tokenizes element and attribute names while parsing.
 */
template<typename _Handler>
class sax_token_parser
{
public:
    typedef _Handler    handler_type;

    sax_token_parser(
        const char* content, const size_t size, const tokens& _tokens,
        xmlns_context& ns_cxt, handler_type& handler);

    ~sax_token_parser();

    void parse();

private:

    /**
     * Re-route callbacks from the internal sax_parser into the token_parser
     * callbacks.
     */
    class handler_wrapper : public sax_token_handler_wrapper_base
    {
        handler_type& m_handler;

    public:
        handler_wrapper(const tokens& _tokens, handler_type& handler) :
            sax_token_handler_wrapper_base(_tokens), m_handler(handler) {}

        void doctype(const sax::doctype_declaration&) {}

        void start_declaration(const pstring&) {}

        void end_declaration(const pstring&)
        {
            m_elem.attrs.clear();
        }

        void start_element(const sax_ns_parser_element& elem)
        {
            set_element(elem);
            m_handler.start_element(m_elem);
            m_elem.attrs.clear();
        }

        void end_element(const sax_ns_parser_element& elem)
        {
            set_element(elem);
            m_handler.end_element(m_elem);
        }

        void characters(const pstring& val, bool transient)
        {
            m_handler.characters(val, transient);
        }
    };

private:
    handler_wrapper m_wrapper;
    sax_ns_parser<handler_wrapper> m_parser;
};

template<typename _Handler>
sax_token_parser<_Handler>::sax_token_parser(
    const char* content, const size_t size, const tokens& _tokens, xmlns_context& ns_cxt, handler_type& handler) :
    m_wrapper(_tokens, handler),
    m_parser(content, size, ns_cxt, m_wrapper)
{
}

template<typename _Handler>
sax_token_parser<_Handler>::~sax_token_parser()
{
}

template<typename _Handler>
void sax_token_parser<_Handler>::parse()
{
    m_parser.parse();
}

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
