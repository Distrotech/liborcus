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
#include <sstream>
#include <exception>
#include <string>

#define DEBUG_SAX_PARSER 1

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

    const char* indent() const;

    size_t cur_pos() const { return m_pos; }

    void next() { ++m_pos; }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        if (m_nest_level == 0)
            throw malformed_xml_error("nest level is about to go below zero.");
        --m_nest_level; 
    }

    char_type cur_char() const { return m_content[m_pos]; }
    char_type next_char() { return m_content[++m_pos]; }

    void blank();

    /**
     * Parse XML header that occurs at the beginning of every XML stream i.e. 
     * <?xml version="..." encoding="..." ?> 
     */
    void header();

    void element();
    void content();
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
    element();
    cout << "finished parsing" << endl;
}

template<typename _Char, typename _Handler>
const char* sax_parser<_Char,_Handler>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str().c_str();
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

    cout << "<?xml " << endl;

    next();
    blank();
    while (cur_char() != '?')
    {
        attribute();
        blank();
    }
    if (next_char() != '>')
        throw malformed_xml_error("xml header must end with '?>'.");

    cout << "?>" << endl;
    next();
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::element()
{
    using namespace std;

    // <elem attr="val" attr="val" ... />
    //
    // <elem attr="val" attr="val" ... > content </elem>

    // Parse the opening element first.  Note that this may contain zero or 
    // more attributes, and may close itself.

    char_type c = cur_char();
    if (c != '<')
        throw malformed_xml_error("element must start with a '<'.");

    next();
    string open_elem_name;
    name(open_elem_name);
    cout << indent() << "<" << open_elem_name << endl;
    while (true)
    {
        blank();
        c = cur_char();
        if (c == '/')
        {
            // Self-closing element: <element/>
            if (next_char() != '>')
                throw malformed_xml_error("expected '/>' to self-close the element.");
            next();
            cout << indent() << "/>" << endl;
            break;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            cout << indent() << ">" << endl;
            nest_up();
            break;
        }
        else
            attribute();
    }

    // Parse the content of this element.

    content();

    // Parse the closing element.

    c = cur_char();
    if (c != '<' || next_char() != '/')
        throw malformed_xml_error("expected '</' at the beginning of a closing element.");
    next();
    string close_elem_name;
    name(close_elem_name);
    if (open_elem_name != close_elem_name)
        throw malformed_xml_error("names of opening and closing elements don't match.");
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::content()
{
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

    cout << indent() << "  attribute: " << _name << "=\"" << _value << "\"" << endl;
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
        str.push_back(c);
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
        str.push_back(c);
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
