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
#include <string>

#include "pstring.hpp"

#define DEBUG_SAX_PARSER 1

namespace orcus {

/** 
 * Template-based sax parser that doesn't use function pointer for 
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Char, typename _Handler, typename _Tokens>
class sax_parser
{
public:
    typedef _Char       char_type;
    typedef _Handler    handler_type;
    typedef _Tokens     tokens;
    typedef typename tokens::token_type     token_type;
    typedef typename tokens::nstoken_type   nstoken_type;

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

    sax_parser(const char_type* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:

    ::std::string indent() const;

    size_t cur_pos() const { return m_pos; }

    void next() { ++m_pos; }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
//      if (m_nest_level == 0)
//          throw malformed_xml_error("nest level is about to go below zero.");
        --m_nest_level; 
    }

    char_type cur_char() const 
    {
//      if (m_pos >= m_size)
//          throw malformed_xml_error("xml stream ended prematurely.");
        return m_content[m_pos]; 
    }

    char_type next_char()
    {
        ++m_pos;
//      if (m_pos >= m_size)
//          throw malformed_xml_error("xml stream ended prematurely.");
        return m_content[m_pos]; 
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

    static bool is_blank(char_type c);
    static bool is_alpha(char_type c);
    static bool is_name_char(char_type c);
    static bool is_numeric(char_type c);

private:
    const char_type* m_content;
    const size_t m_size;
    size_t m_pos;
    size_t m_nest_level;
    handler_type& m_handler;
};

template<typename _Char, typename _Handler, typename _Tokens>
sax_parser<_Char,_Handler,_Tokens>::sax_parser(const char_type* content, const size_t size, handler_type& handler) :
    m_content(content), m_size(size), m_pos(0), m_nest_level(0), m_handler(handler)
{
}

template<typename _Char, typename _Handler, typename _Tokens>
sax_parser<_Char,_Handler,_Tokens>::~sax_parser()
{
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::parse()
{
    using namespace std;
    m_pos = 0;
    m_nest_level = 0;
    header();
    blank();
    body();
    cout << "finished parsing" << endl;
}

template<typename _Char, typename _Handler, typename _Tokens>
::std::string sax_parser<_Char,_Handler,_Tokens>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str();
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::blank()
{
    char_type c = cur_char();
    while (is_blank(c))
        c = next_char();
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::header()
{
    using namespace std;

    char_type c = cur_char();
    if (c != '<' || next_char() != '?' || next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
        throw malformed_xml_error("xml header must begin with '<?xml'.");

#if DEBUG_SAX_PARSER
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
    cout << "?>" << endl;
#endif
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::body()
{
    while (m_pos < m_size)
    {
        if (cur_char() == '<')
            element();
        else
            characters();
    }
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::element()
{
    assert(cur_char() == '<');
    char_type c = next_char();
    if (c == '/')
        element_close();
    else
        element_open();
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::element_open()
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
        char_type c = cur_char();
        if (c == '/')
        {
            // Self-closing element: <element/>
            if (next_char() != '>')
                throw malformed_xml_error("expected '/>' to self-close the element.");
            next();
#if DEBUG_SAX_PARSER
            cout << indent() << "/>" << endl;
#endif
            return;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            nest_up();
#if DEBUG_SAX_PARSER
            cout << indent() << ">" << endl;
#endif
            return;
        }
        else
            attribute();
    }
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::element_close()
{
    using namespace std;
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

#if DEBUG_SAX_PARSER
    cout << indent() << "</" << tokens::get_nstoken_name(elem_nstoken) << ":" << tokens::get_token_name(elem_token) << ">" << endl;
#endif
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::characters()
{
    size_t first = m_pos;
    char_type c = cur_char();
    while (c != '<')
        c = next_char();

    if (m_pos > first)
    {
        size_t size = m_pos - first;
        pstring val(reinterpret_cast<const char*>(m_content) + first, size);
#if DEBUG_SAX_PARSER
        using namespace std;
        cout << indent() << val.str() << endl;
#endif
    }
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::attribute()
{
    using namespace std;
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

    char_type c = cur_char();
    if (c != '=')
        throw malformed_xml_error("attribute must begin with 'name=..");
    next();
    value(_value);

#if DEBUG_SAX_PARSER
    cout << indent() << "  attribute: " << tokens::get_nstoken_name(ns_token) << ":" << tokens::get_token_name(name_token) << "=\"" << _value.str() << "\"" << endl;
#endif
}

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::name(pstring& str)
{
    size_t first = m_pos;
    char_type c = cur_char();
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

template<typename _Char, typename _Handler, typename _Tokens>
void sax_parser<_Char,_Handler,_Tokens>::value(pstring& str)
{
    char_type c = cur_char();
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

template<typename _Char, typename _Handler, typename _Tokens>
bool sax_parser<_Char,_Handler,_Tokens>::is_blank(char_type c)
{
    if (c == ' ')
        return true;
    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;
    return false;
}

template<typename _Char, typename _Handler, typename _Tokens>
bool sax_parser<_Char,_Handler,_Tokens>::is_alpha(char_type c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Char, typename _Handler, typename _Tokens>
bool sax_parser<_Char,_Handler,_Tokens>::is_name_char(char_type c)
{
    if (c == '-')
        return true;

    return false;
}

template<typename _Char, typename _Handler, typename _Tokens>
bool sax_parser<_Char,_Handler,_Tokens>::is_numeric(char_type c)
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
