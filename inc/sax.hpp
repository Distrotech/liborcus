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

#define DEBUG_SAX_PARSER 0

namespace orcus {

/** 
 * Template-based sax parser that doesn't use function pointer for 
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Char, typename _Handler>
class sax_parser
{
public:
    typedef _Char       char_type;
    typedef _Handler    handler_type;

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

    void name(::std::string& str);
    void value(::std::string& str);

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

template<typename _Char, typename _Handler>
sax_parser<_Char,_Handler>::sax_parser(const char_type* content, const size_t size, handler_type& handler) :
    m_content(content), m_size(size), m_pos(0), m_nest_level(0), m_handler(handler)
{
}

template<typename _Char, typename _Handler>
sax_parser<_Char,_Handler>::~sax_parser()
{
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::parse()
{
    using namespace std;
    m_pos = 0;
    m_nest_level = 0;
    header();
    blank();
    body();
    cout << "finished parsing" << endl;
}

template<typename _Char, typename _Handler>
::std::string sax_parser<_Char,_Handler>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str();
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::blank()
{
    char_type c = cur_char();
    while (is_blank(c))
        c = next_char();
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::header()
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

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::body()
{
    while (m_pos < m_size)
    {
        if (cur_char() == '<')
            element();
        else
            characters();
    }
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::element()
{
    assert(cur_char() == '<');
    char_type c = next_char();
    if (c == '/')
        element_close();
    else
        element_open();
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::element_open()
{
    using namespace std;
    assert(is_alpha(cur_char()));

    string open_elem_name;
    name(open_elem_name);
#if DEBUG_SAX_PARSER
    cout << indent() << "<" << open_elem_name << endl;
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

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::element_close()
{
    using namespace std;
    assert(cur_char() == '/');
    nest_down();
    next();
    string close_elem_name;
    name(close_elem_name);
    if (cur_char() != '>')
        throw malformed_xml_error("expected '>' to close the element.");
    next();

#if DEBUG_SAX_PARSER
    cout << indent() << "</" << close_elem_name << ">" << endl;
#endif
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::characters()
{
    using namespace std;
    string buf;
    char_type c = cur_char();
    while (c != '<')
    {
//      buf.push_back(c);
        c = next_char();
    }
#if DEBUG_SAX_PARSER
    cout << indent() << buf << endl;
#endif
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::attribute()
{
    using namespace std;
    string _name, _value;
    name(_name);
    char_type c = cur_char();
    if (c != '=')
        throw malformed_xml_error("attribute must begin with 'name=..");
    next();
    value(_value);

#if DEBUG_SAX_PARSER
    cout << indent() << "  attribute: " << _name << "=\"" << _value << "\"" << endl;
#endif
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::name(::std::string& str)
{
    char_type c = cur_char();
    if (!is_alpha(c))
    {
        ::std::ostringstream os;
        os << "name must begin with an alphabet, but got this instead '" << c << "'";
        throw malformed_xml_error(os.str());
    }

    while (is_alpha(c) || is_numeric(c) || is_name_char(c))
    {
//      str.push_back(c);
        c = next_char();
    }
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::value(::std::string& str)
{
    char_type c = cur_char();
    if (c != '"')
        throw malformed_xml_error("attribute value must be quoted");

    c = next_char();
    while (c != '"')
    {
//      str.push_back(c);
        c = next_char();
    }
    next();
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_blank(char_type c)
{
    if (c == ' ')
        return true;
    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;
    return false;
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_alpha(char_type c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_name_char(char_type c)
{
    if (c == ':' || c == '-')
        return true;

    return false;
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_numeric(char_type c)
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
