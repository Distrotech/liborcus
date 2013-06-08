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

#ifndef __ORCUS_SAX_PARSER_HPP__
#define __ORCUS_SAX_PARSER_HPP__

#include <exception>
#include <cassert>
#include <sstream>

#include "pstring.hpp"
#include "cell_buffer.hpp"
#include "sax_parser_global.hpp"

#define ORCUS_DEBUG_SAX_PARSER 0
#include <iostream>
using std::cout;

#if ORCUS_DEBUG_SAX_PARSER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus {

class malformed_xml_error : public std::exception
{
public:
    malformed_xml_error(const std::string& msg) : m_msg(msg) {}
    virtual ~malformed_xml_error() throw() {}
    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

/**
 * Element properties passed by sax_parser to its handler's open_element()
 * and close_element() calls.
 */
struct sax_parser_element
{
    pstring ns;            // element namespace (optional)
    pstring name;          // element name
    const char* begin_pos; // position of the opening brace '<'.
    const char* end_pos;   // position of the char after the closing brace '>'.
};

/**
 * Template-based sax parser that doesn't use function pointer for
 * callbacks for better performance, especially on large XML streams.
 */
template<typename _Handler>
class sax_parser
{
public:
    typedef _Handler handler_type;

    sax_parser(const char* content, const size_t size, handler_type& handler);
    ~sax_parser();

    void parse();

private:

    std::string indent() const;

    void next() { ++m_pos; ++m_char; }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        assert(m_nest_level > 0);
        --m_nest_level;
    }

    inline bool has_char() const { return m_pos < m_size; }

    inline size_t remains() const
    {
#if ORCUS_DEBUG_SAX_PARSER
        if (m_pos >= m_size)
            throw malformed_xml_error("xml stream ended prematurely.");
#endif
        return m_size - m_pos;
    }

    char cur_char() const
    {
#if ORCUS_DEBUG_SAX_PARSER
        if (m_pos >= m_size)
            throw malformed_xml_error("xml stream ended prematurely.");
#endif
        return *m_char;
    }

    char next_char()
    {
        next();
#if ORCUS_DEBUG_SAX_PARSER
        if (m_pos >= m_size)
            throw malformed_xml_error("xml stream ended prematurely.");
#endif
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
    void element_open(const char* begin_pos);
    void element_close(const char* begin_pos);
    void special_tag();
    void comment();
    void content();
    void characters();
    void characters_with_encoded_char();
    void attribute();

    /**
     * Skip an optional byte order mark at the begining of the xml stream.
     */
    void skip_bom();

    void parse_encoded_char();

    void name(pstring& str);

    /**
     * Parse attribute value.  Note that the retreived string may be stored in
     * the temporary cell buffer. Use the string immediately after this call
     * before the buffer becomes invalid.
     */
    void value(pstring& str);
    void value_with_encoded_char(pstring& str);

    static bool is_blank(char c);
    static bool is_alpha(char c);
    static bool is_name_char(char c);
    static bool is_numeric(char c);

private:
    cell_buffer m_cell_buf;
    const char* m_content;
    const char* m_char;
    const size_t m_size;
    size_t m_pos;
    size_t m_nest_level;
    bool m_root_elem_open:1;
    handler_type& m_handler;
};

template<typename _Handler>
sax_parser<_Handler>::sax_parser(
    const char* content, const size_t size, handler_type& handler) :
    m_content(content),
    m_char(content),
    m_size(size),
    m_pos(0),
    m_nest_level(0),
    m_root_elem_open(true),
    m_handler(handler)
{
}

template<typename _Handler>
sax_parser<_Handler>::~sax_parser()
{
}

template<typename _Handler>
void sax_parser<_Handler>::parse()
{
    m_pos = 0;
    m_nest_level = 0;
    m_char = m_content;
    header();
    blank();
    body();
}

template<typename _Handler>
::std::string sax_parser<_Handler>::indent() const
{
    ::std::ostringstream os;
    for (size_t i = 0; i < m_nest_level; ++i)
        os << "  ";
    return os.str();
}

template<typename _Handler>
void sax_parser<_Handler>::blank()
{
    char c = cur_char();
    while (is_blank(c))
        c = next_char();
}

template<typename _Handler>
void sax_parser<_Handler>::header()
{
    // we don't handle multi byte encodings so we can just skip bom entry if exists.
    skip_bom();

    char c = cur_char();
    if (c != '<' || next_char() != '?' || next_char() != 'x' || next_char() != 'm' || next_char() != 'l')
        throw malformed_xml_error("xml header must begin with '<?xml'.");

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

    m_handler.declaration();
}

template<typename _Handler>
void sax_parser<_Handler>::skip_bom()
{
    // 0xef 0xbb 0 xbf is the UTF-8 byte order mark
    unsigned char c = static_cast<unsigned char>(cur_char());
    if (c != '<')
    {
        if (c != 0xef || static_cast<unsigned char>(next_char()) != 0xbb ||
            static_cast<unsigned char>(next_char()) != 0xbf || next_char() != '<')
            throw malformed_xml_error("unsupported encoding. only 8 bit encodings are supported");
    }
}

template<typename _Handler>
void sax_parser<_Handler>::body()
{
    while (has_char())
    {
        if (cur_char() == '<')
        {
            element();
            if (!m_root_elem_open)
                // Root element closed.  Stop parsing.
                return;
        }
        else if (m_nest_level)
            // Call characters only when in xml hierarchy.
            characters();
        else
            next();
    }
}

template<typename _Handler>
void sax_parser<_Handler>::element()
{
    assert(cur_char() == '<');
    const char* pos = m_char;
    char c = next_char();
    switch (c)
    {
        case '/':
            element_close(pos);
        break;
        case '!':
            special_tag();
        break;
        default:
            if (!is_alpha(c))
                throw malformed_xml_error("expected an alphabet.");
            element_open(pos);
    }
}

template<typename _Handler>
void sax_parser<_Handler>::element_open(const char* begin_pos)
{
    assert(is_alpha(cur_char()));

    sax_parser_element elem;
    elem.begin_pos = begin_pos;

    name(elem.name);
    if (cur_char() == ':')
    {
        // this element name is namespaced.
        elem.ns = elem.name;
        next();
        name(elem.name);
    }

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
            elem.end_pos = m_char;
            m_handler.start_element(elem);
            m_handler.end_element(elem);
            return;
        }
        else if (c == '>')
        {
            // End of opening element: <element>
            next();
            elem.end_pos = m_char;
            nest_up();
            m_handler.start_element(elem);
            return;
        }
        else
            attribute();
    }
}

template<typename _Handler>
void sax_parser<_Handler>::element_close(const char* begin_pos)
{
    assert(cur_char() == '/');
    nest_down();
    next();
    sax_parser_element elem;
    elem.begin_pos = begin_pos;

    name(elem.name);
    if (cur_char() == ':')
    {
        elem.ns = elem.name;
        next();
        name(elem.name);
    }

    if (cur_char() != '>')
        throw malformed_xml_error("expected '>' to close the element.");
    next();
    elem.end_pos = m_char;

    m_handler.end_element(elem);
    if (!m_nest_level)
        m_root_elem_open = false;
}

template<typename _Handler>
void sax_parser<_Handler>::special_tag()
{
    assert(cur_char() == '!');
    // This can be either <![CDATA, <!--, or <!DOCTYPE.
    size_t len = remains();
    if (len < 2)
        throw malformed_xml_error("special tag too short.");

    switch (next_char())
    {
        case '-':
        {
            // Possibly comment.
            if (next_char() != '-')
                throw malformed_xml_error("comment expected.");

            len = remains();
            if (len < 3)
                throw malformed_xml_error("malformed comment.");

            next();
            comment();
        }
        break;
        default:
            // TODO: Handle CDATA and DOCTYPE.
            throw malformed_xml_error("failed to parse special tag.");
    }
}

template<typename _Handler>
void sax_parser<_Handler>::comment()
{
    // Parse until we reach '-->'.
    size_t len = remains();
    assert(len > 3);
    char c = cur_char();
    size_t i = 0;
    bool hyphen = false;
    for (; i < len; ++i, c = next_char())
    {
        if (c == '-')
        {
            if (!hyphen)
                // first hyphen.
                hyphen = true;
            else
                // second hyphen.
                break;
        }
        else
            hyphen = false;
    }

    if (len - i < 2 || next_char() != '>')
        throw malformed_xml_error("'--' should not occur in comment other than in the closing tag.");

    next();
}

template<typename _Handler>
void sax_parser<_Handler>::characters()
{
    size_t first = m_pos;
    const char* p0 = m_char;
    for (; has_char(); next())
    {
        if (cur_char() == '<')
            break;

        if (cur_char() == '&')
        {
            // Text span with one or more encoded characters. Parse using cell buffer.
            m_cell_buf.reset();
            m_cell_buf.append(p0, m_pos-first);
            characters_with_encoded_char();
            return;
        }
    }

    if (m_pos > first)
    {
        size_t size = m_pos - first;
        pstring val(m_content + first, size);
        m_handler.characters(val);
    }
}

template<typename _Handler>
void sax_parser<_Handler>::characters_with_encoded_char()
{
    assert(cur_char() == '&');
    parse_encoded_char();
    assert(cur_char() != ';');

    size_t first = m_pos;

    while (has_char())
    {
        if (cur_char() == '&')
        {
            if (m_pos > first)
                m_cell_buf.append(m_content+first, m_pos-first);

            parse_encoded_char();
            first = m_pos;
        }

        if (cur_char() == '<')
            break;

        if (cur_char() != '&')
            next();
    }

    if (m_pos > first)
        m_cell_buf.append(m_content+first, m_pos-first);

    if (m_cell_buf.empty())
        m_handler.characters(pstring());
    else
        m_handler.characters(pstring(m_cell_buf.get(), m_cell_buf.size()));
}

template<typename _Handler>
void sax_parser<_Handler>::attribute()
{
    pstring attr_ns_name, attr_name, attr_value;
    name(attr_name);
    if (cur_char() == ':')
    {
        // Attribute name is namespaced.
        attr_ns_name = attr_name;
        next();
        name(attr_name);
    }

#if ORCUS_DEBUG_SAX_PARSER
    cout << "attribute: ns='" << attr_ns_name << "', name='" << attr_name << "'" << endl;
#endif

    char c = cur_char();
    if (c != '=')
    {
        std::ostringstream os;
        os << "Attribute must begin with 'name=..'. (ns='" << attr_ns_name << "', name='" << attr_name << "')";
        throw malformed_xml_error(os.str());
    }

    next();
    value(attr_value);

    m_handler.attribute(attr_ns_name, attr_name, attr_value);
}

template<typename _Handler>
void sax_parser<_Handler>::parse_encoded_char()
{
    assert(cur_char() == '&');
    next();
    const char* p0 = m_char;
    for (; has_char(); next())
    {
        if (cur_char() != ';')
            continue;

        size_t n = m_char - p0;
        if (!n)
            throw malformed_xml_error("empty encoded character.");

#if ORCUS_DEBUG_SAX_PARSER
        cout << "sax_parser::parse_encoded_char: raw='" << std::string(p0, n) << "'" << endl;
#endif

        char c = decode_xml_encoded_char(p0, n);
        if (c)
            m_cell_buf.append(&c, 1);

        // Move to the character past ';' before returning to the parent call.
        next();

        if (!c)
        {
#if ORCUS_DEBUG_SAX_PARSER
            cout << "sax_parser::parse_encoded_char: not a known encoding name. Use the original." << endl;
#endif
            // Unexpected encoding name. Use the original text.
            m_cell_buf.append(p0, m_char-p0);
        }

        return;
    }

    throw malformed_xml_error("error parsing encoded character: terminating character is not found.");
}

template<typename _Handler>
void sax_parser<_Handler>::name(pstring& str)
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
    str = pstring(m_content+first, size);
}

template<typename _Handler>
void sax_parser<_Handler>::value(pstring& str)
{
    char c = cur_char();
    if (c != '"')
        throw malformed_xml_error("attribute value must be quoted");

    c = next_char();
    size_t first = m_pos;
    const char* p0 = m_char;

    for (; c != '"'; c = next_char())
    {
        if (c == '&')
        {
            // This value contains one or more encoded characters.
            m_cell_buf.reset();
            m_cell_buf.append(p0, m_pos-first);
            value_with_encoded_char(str);
            return;
        }
    }

    str = pstring(p0, m_pos-first);

    // Skip the closing quote.
    next();
}

template<typename _Handler>
void sax_parser<_Handler>::value_with_encoded_char(pstring& str)
{
    assert(cur_char() == '&');
    parse_encoded_char();
    assert(cur_char() != ';');

    size_t first = m_pos;

    while (has_char())
    {
        if (cur_char() == '&')
        {
            if (m_pos > first)
                m_cell_buf.append(m_content+first, m_pos-first);

            parse_encoded_char();
            assert(cur_char() != ';');
            first = m_pos;
        }

        if (cur_char() == '"')
            break;

        if (cur_char() != '&')
            next();
    }

    if (m_pos > first)
        m_cell_buf.append(m_content+first, m_pos-first);

    if (!m_cell_buf.empty())
        str = pstring(m_cell_buf.get(), m_cell_buf.size());

    // Skip the closing quote.
    assert(cur_char() == '"');
    next();
}

template<typename _Handler>
bool sax_parser<_Handler>::is_blank(char c)
{
    if (c == ' ')
        return true;
    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;
    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_alpha(char c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_name_char(char c)
{
    switch (c)
    {
        case '-':
        case '_':
            return true;
    }

    return false;
}

template<typename _Handler>
bool sax_parser<_Handler>::is_numeric(char c)
{
    if ('0' <= c && c <= '9')
        return true;
    return false;
}

}

#endif
