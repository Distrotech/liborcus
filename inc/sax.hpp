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

#ifndef __ORCUS_SAX_HPP__
#define __ORCUS_SAX_HPP__

#include <iostream>
#include <cassert>
#include <sstream>
#include <exception>
#include <vector>
#include <algorithm>

#include "pstring.hpp"

#define DEBUG_SAX_PARSER 0

namespace orcus {

namespace sax {

template<typename _Attr, typename _Tokens>
class attr_printer : public ::std::unary_function<_Attr, void>
{
public:
    attr_printer(const ::std::string& indent) : m_indent(indent) {}

    void operator() (const _Attr& attr) const
    {
        using namespace std;
        cout << m_indent << "  attribute: " 
            << _Tokens::get_nstoken_name(attr.ns) << ":" 
            << _Tokens::get_token_name(attr.name) << "=\"" 
            << attr.value.str() << "\"" << endl;
    }
private:
    ::std::string m_indent;
};

}

/** 
 * Template-based sax parser that doesn't use function pointer for 
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Handler, typename _Tokens>
class sax_parser
{
public:
    typedef _Handler    handler_type;
    typedef _Tokens     tokens;
    typedef typename tokens::token_type     token_type;
    typedef typename tokens::nstoken_type   nstoken_type;
    typedef typename tokens::attr_type      attr_type;

    class malformed_xml_error : public ::std::exception
    {
    public:
        malformed_xml_error(const ::std::string& msg) : m_msg(msg) {}
        virtual ~malformed_xml_error() throw() {}
        virtual const char* what() const throw()
        {
            return m_msg.c_str();
        }
    private:
        ::std::string m_msg;
    };

    sax_parser(const char* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:

    ::std::string indent() const;

    void next() { ++m_pos; ++m_char; }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        assert(m_nest_level > 0);
        --m_nest_level; 
    }

    char cur_char() const 
    {
//      if (m_pos >= m_size)
//          throw malformed_xml_error("xml stream ended prematurely.");
        return *m_char;
    }

    char next_char()
    {
        next();
//      if (m_pos >= m_size)
//          throw malformed_xml_error("xml stream ended prematurely.");
        return *m_char;
    }

    void blank();

    /**
     * Parse XML header that occurs at the beginning of every XML stream i.e. 
     * <?xml version="..." encoding="..." ?> 
     */
    void header();
    void body();
    void element();
    void element_open();
    void element_close();
    void content();
    void characters();
    void attribute();

    void name(pstring& str);
    void value(pstring& str);

    void clear_attributes();
    void print_attributes() const;

    static bool is_blank(char c);
    static bool is_alpha(char c);
    static bool is_name_char(char c);
    static bool is_numeric(char c);

private:
    ::std::vector<attr_type> m_attrs;
    const char* m_content;
    const char* m_char;
    const size_t m_size;
    size_t m_pos;
    size_t m_nest_level;
    handler_type& m_handler;
};

template<typename _Handler, typename _Tokens>
sax_parser<_Handler,_Tokens>::sax_parser(const char* content, const size_t size, handler_type& handler) :
    m_content(content), m_char(content), m_size(size), m_pos(0), m_nest_level(0), m_handler(handler)
{
}

template<typename _Handler, typename _Tokens>
sax_parser<_Handler,_Tokens>::~sax_parser()
{
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::parse()
{
    using namespace std;
    m_pos = 0;
    m_nest_level = 0;
    m_char = m_content;
    header();
    blank();
    body();
    cout << "finished parsing" << endl;
}

template<typename _Handler, typename _Tokens>
::std::string sax_parser<_Handler,_Tokens>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::blank()
{
    char c = cur_char();
    while (is_blank(c))
        c = next_char();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::header()
{
    char c = cur_char();
    if (c != '<' || next_char() != '?' || next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
        throw malformed_xml_error("xml header must begin with '<?xml'.");

#if DEBUG_SAX_PARSER
    using namespace std;
    cout << "<?xml " << endl;
#endif

    next();
    blank();
    while (cur_char() != '?')
    {
        attribute();
        blank();
    }
    if (next_char() != '>')
        throw malformed_xml_error("xml header must end with '?>'.");

    next();
#if DEBUG_SAX_PARSER
    print_attributes();
    cout << "?>" << endl;
#endif
    clear_attributes();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::body()
{
    while (m_pos < m_size)
    {
        if (cur_char() == '<')
            element();
        else
            characters();
    }
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::element()
{
    assert(cur_char() == '<');
    char c = next_char();
    if (c == '/')
        element_close();
    else
        element_open();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::element_open()
{
    assert(is_alpha(cur_char()));

    pstring elem_name;
    token_type elem_token     = tokens::XML_UNKNOWN_TOKEN;
    nstoken_type elem_nstoken = tokens::XMLNS_UNKNOWN_TOKEN;
    name(elem_name);
    if (cur_char() == ':')
    {
        elem_nstoken = tokens::get_nstoken(elem_name);
        next();
        name(elem_name);
        elem_token = tokens::get_token(elem_name);
    }
    else
        elem_token = tokens::get_token(elem_name);

#if DEBUG_SAX_PARSER
    using namespace std;
    cout << indent() << "<" << tokens::get_nstoken_name(elem_nstoken) << ":" << tokens::get_token_name(elem_token) << endl;
#endif
    while (true)
    {
        blank();
        char c = cur_char();
        if (c == '/')
        {
            // Self-closing element: <element/>
            if (next_char() != '>')
                throw malformed_xml_error("expected '/>' to self-close the element.");
            next();
            m_handler.start_element(elem_nstoken, elem_token, m_attrs);
#if DEBUG_SAX_PARSER
            print_attributes();
            cout << indent() << "/>" << endl;
#endif
            clear_attributes();
            m_handler.end_element(elem_nstoken, elem_token);
            return;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            m_handler.start_element(elem_nstoken, elem_token, m_attrs);
#if DEBUG_SAX_PARSER
            print_attributes();
            cout << indent() << ">" << endl;
#endif
            clear_attributes();
            nest_up();
            return;
        }
        else
            attribute();
    }
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::element_close()
{
    assert(cur_char() == '/');
    nest_down();
    next();
    pstring elem_name;
    token_type elem_token     = tokens::XML_UNKNOWN_TOKEN;
    nstoken_type elem_nstoken = tokens::XMLNS_UNKNOWN_TOKEN;;
    name(elem_name);
    if (cur_char() == ':')
    {
        elem_nstoken = tokens::get_nstoken(elem_name);
        next();
        name(elem_name);
        elem_token = tokens::get_token(elem_name);
    }
    else
        elem_token = tokens::get_token(elem_name);

    if (cur_char() != '>')
        throw malformed_xml_error("expected '>' to close the element.");
    next();

    m_handler.end_element(elem_nstoken, elem_token);
#if DEBUG_SAX_PARSER
    using namespace std;
    cout << indent() << "</" << tokens::get_nstoken_name(elem_nstoken) << ":" << tokens::get_token_name(elem_token) << ">" << endl;
#endif
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::characters()
{
    size_t first = m_pos;
    char c = cur_char();
    while (c != '<')
        c = next_char();

    if (m_pos > first)
    {
        size_t size = m_pos - first;
        pstring val(reinterpret_cast<const char*>(m_content) + first, size);
        m_handler.characters(val);
#if DEBUG_SAX_PARSER
        using namespace std;
        cout << indent() << val.str() << endl;
#endif
    }
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::attribute()
{
    pstring _name, _value;
    nstoken_type ns_token = tokens::XMLNS_UNKNOWN_TOKEN;
    token_type name_token = tokens::XML_UNKNOWN_TOKEN;
    name(_name);
    if (cur_char() == ':')
    {
        // Attribute name is namespaced.
        ns_token = tokens::get_nstoken(_name);
        next();
        name(_name);
        name_token = tokens::get_token(_name);
    }
    else
        // Attribute name is without namespace.
        name_token = tokens::get_token(_name);

    char c = cur_char();
    if (c != '=')
        throw malformed_xml_error("attribute must begin with 'name=..");
    next();
    value(_value);

    m_attrs.push_back(attr_type(ns_token, name_token, _value));
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::name(pstring& str)
{
    size_t first = m_pos;
    char c = cur_char();
    if (!is_alpha(c))
    {
        ::std::ostringstream os;
        os << "name must begin with an alphabet, but got this instead '" << c << "'";
        throw malformed_xml_error(os.str());
    }

    while (is_alpha(c) || is_numeric(c) || is_name_char(c))
        c = next_char();

    size_t size = m_pos - first;
    str = pstring(reinterpret_cast<const char*>(m_content) + first, size);
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::value(pstring& str)
{
    char c = cur_char();
    if (c != '"')
        throw malformed_xml_error("attribute value must be quoted");

    c = next_char();
    size_t first = m_pos;
    while (c != '"')
        c = next_char();

    size_t size = m_pos - first;
    str = pstring(reinterpret_cast<const char*>(m_content) + first, size);

    // Skip the closing quote.
    next();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::clear_attributes()
{
    m_attrs.clear();
}

template<typename _Handler, typename _Tokens>
void sax_parser<_Handler,_Tokens>::print_attributes() const
{
    using namespace std;
    for_each(m_attrs.begin(), m_attrs.end(), sax::attr_printer<attr_type, tokens>(indent()));
}

template<typename _Handler, typename _Tokens>
bool sax_parser<_Handler,_Tokens>::is_blank(char c)
{
    if (c == ' ')
        return true;
    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;
    return false;
}

template<typename _Handler, typename _Tokens>
bool sax_parser<_Handler,_Tokens>::is_alpha(char c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Handler, typename _Tokens>
bool sax_parser<_Handler,_Tokens>::is_name_char(char c)
{
    if (c == '-')
        return true;

    return false;
}

template<typename _Handler, typename _Tokens>
bool sax_parser<_Handler,_Tokens>::is_numeric(char c)
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
