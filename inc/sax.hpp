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

    size_t cur_pos() const { return m_pos; }

    void inc() { ++m_pos; }

    char_type cur_char() const { return m_content[m_pos]; }
    char_type next_char() { return m_content[++m_pos]; }

    void blank();

    /**
     * <?xml .... ?>
     */
    void header();
    void attribute();
    void open_brace();
    void close_brace();

    void name(::std::string& str);
    void value(::std::string& str);

    bool is_alpha(char_type c) const;
    bool is_numeric(char_type c) const;

private:
    const char_type* m_content;
    const size_t m_size;
    size_t m_pos;
    handler_type& m_handler;
};

template<typename _Char, typename _Handler>
sax_parser<_Char,_Handler>::sax_parser(const char_type* content, const size_t size, handler_type& handler) :
    m_content(content), m_size(size), m_pos(0), m_handler(handler)
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
    header();
    cout << "finished parsing" << endl;
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::blank()
{
    using namespace std;

    while (next_char() == ' ')
        ;
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::header()
{
    using namespace std;

    char_type c = cur_char();
    if (c != '<' || next_char() != '?' || next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
        throw malformed_xml_error("xml header must begin with '<?xml'.");

    blank();
    while (cur_char() != '?')
    {
        attribute();
        blank();
    }
    if (next_char() != '>')
        throw malformed_xml_error("xml header must end with '?>'.");

    inc();
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
    inc();
    value(_value);
    cout << "attribute: " << _name << "=\"" << _value << "\"" << endl;
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::open_brace()
{
    using namespace std;

    char_type c = cur_char();
    if (m_pos == 1 && c == '?')
    {
        // <?xml ... ?>
        if (next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
            throw malformed_xml_error("expected <?xml ...");

        blank();
        attribute();
    }
}

template<typename _Char, typename _Handler>
void sax_parser<_Char,_Handler>::close_brace()
{
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

    while (is_alpha(c) || is_numeric(c))
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
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_alpha(char_type c) const
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Char, typename _Handler>
bool sax_parser<_Char,_Handler>::is_numeric(char_type c) const
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
