/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

#ifndef __ORCUS_SAX_TOKEN_PARSER_HPP__
#define __ORCUS_SAX_TOKEN_PARSER_HPP__

#include <vector>
#include <algorithm>
#include <functional>

#include "types.hpp"
#include "sax_parser.hpp"

namespace orcus {

namespace sax {

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
            << m_tokens.get_nstoken_name(attr.ns) << ":"
            << m_tokens.get_token_name(attr.name) << "=\""
            << attr.value.str() << "\"" << endl;
    }
private:
    const _Tokens& m_tokens;
    ::std::string m_indent;
};

}

/**
 * Element properties passed to its handler via start_element() and
 * end_element() calls.
 */
struct sax_token_parser_element
{
    xmlns_token_t ns;
    xml_token_t name;
    std::vector<xml_attr_t> attrs;
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

    sax_token_parser(const char* content, const size_t size, const tokens_map& tokens, handler_type& handler);
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

        void declaration()
        {
            m_elem.attrs.clear();
        }

        void start_element(const sax_parser_element& elem)
        {
            m_elem.ns = tokenize_ns(elem.ns);
            m_elem.name = tokenize(elem.name);
            m_handler.start_element(m_elem);
            m_elem.attrs.clear();
        }

        void end_element(const sax_parser_element& elem)
        {
            m_elem.ns = tokenize_ns(elem.ns);
            m_elem.name = tokenize(elem.name);
            m_handler.end_element(m_elem);
        }

        void characters(const pstring& val)
        {
            m_handler.characters(val);
        }

        void attribute(const pstring& ns, const pstring& name, const pstring& val)
        {
            xml_token_t elem_token = tokenize(name);
            xmlns_token_t elem_nstoken = tokenize_ns(ns);
            m_elem.attrs.push_back(xml_attr_t(elem_nstoken, elem_token, val));
        }

    private:
        xmlns_token_t tokenize_ns(const pstring& ns) const
        {
            xmlns_token_t token = XMLNS_UNKNOWN_TOKEN;
            if (!ns.empty())
                token = m_tokens.get_nstoken(ns);
            return token;
        }

        xml_token_t tokenize(const pstring& name) const
        {
            xml_token_t token = XML_UNKNOWN_TOKEN;
            if (!name.empty())
                token = m_tokens.get_token(name);
            return token;
        }
    };

private:
    handler_wrapper m_wrapper;
    sax_parser<handler_wrapper> m_parser;
};

template<typename _Handler, typename _Tokens>
sax_token_parser<_Handler,_Tokens>::sax_token_parser(
    const char* content, const size_t size, const tokens_map& tokens, handler_type& handler) :
    m_wrapper(tokens, handler),
    m_parser(content, size, m_wrapper)
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
