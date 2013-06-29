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

#include "orcus/sax_parser_base.hpp"

#include <cstring>

namespace orcus { namespace sax {

malformed_xml_error::malformed_xml_error(const std::string& msg) : m_msg(msg) {}

malformed_xml_error::~malformed_xml_error() throw() {}

const char* malformed_xml_error::what() const throw()
{
    return m_msg.c_str();
}

char decode_xml_encoded_char(const char* p, size_t n)
{
    if (n == 2)
    {
        if (!std::strncmp(p, "lt", n))
            return '<';
        else if (!std::strncmp(p, "gt", n))
            return '>';
        else
            return '\0';
    }
    else if (n == 3)
    {
        if (!std::strncmp(p, "amp", n))
            return '&';
        else
            return '\0';
    }
    else if (n == 4)
    {
        if (!std::strncmp(p, "apos", n))
            return '\'';
        else if (!std::strncmp(p, "quot", 4))
            return '"';
        else
            return '\0';
    }

    return '\0';
}

bool is_blank(char c)
{
    if (c == ' ')
        return true;

    if (c == 0x0A || c == 0x0D)
        // LF or CR
        return true;

    if (c == '\t')
        // tab character.
        return true;

    return false;
}

bool is_alpha(char c)
{
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    return false;
}

bool is_name_char(char c)
{
    switch (c)
    {
        case '-':
        case '_':
            return true;
    }

    return false;
}

bool is_numeric(char c)
{
    return ('0' <= c && c <= '9');
}

parser_base::parser_base(const char* content, size_t size) :
    m_content(content),
    m_char(content),
    m_size(size),
    m_pos(0),
    m_nest_level(0),
    m_root_elem_open(true)
{
}

void parser_base::blank()
{
    char c = cur_char();
    while (is_blank(c))
    {
        next();
        if (!has_char())
            return;

        c = cur_char();
    }
}

void parser_base::comment()
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

void parser_base::skip_bom()
{
    if (remains() < 4)
        // Stream too short to have a byte order mark.
        return;

    // 0xef 0xbb 0 xbf is the UTF-8 byte order mark
    unsigned char c = static_cast<unsigned char>(cur_char());
    if (c != '<')
    {
        if (c != 0xef || static_cast<unsigned char>(next_char()) != 0xbb ||
            static_cast<unsigned char>(next_char()) != 0xbf || next_char() != '<')
            throw malformed_xml_error("unsupported encoding. only 8 bit encodings are supported");
    }
}

void parser_base::expects_next(const char* p, size_t n)
{
    if (remains() < n+1)
        throw malformed_xml_error("not enough stream left to check for an expected string segment.");

    const char* p0 = p;
    const char* p_end = p + n;
    char c = next_char();
    for (; p != p_end; ++p, c = next_char())
    {
        if (c == *p)
            continue;

        std::ostringstream os;
        os << "'" << std::string(p0, n) << "' was expected, but not found.";
        throw malformed_xml_error("sadf");
    }
}

void parser_base::parse_encoded_char()
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

void parser_base::value_with_encoded_char(pstring& str)
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

void parser_base::value(pstring& str, bool decode)
{
    char c = cur_char();
    if (c != '"')
        throw malformed_xml_error("value must be quoted");

    c = next_char_checked();
    size_t first = m_pos;
    const char* p0 = m_char;

    for (;c != '"'; c = next_char_checked())
    {
        if (decode && c == '&')
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

void parser_base::name(pstring& str)
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
        c = next_char_checked();

    size_t size = m_pos - first;
    str = pstring(m_content+first, size);
}

void parser_base::element_name(parser_element& elem, const char* begin_pos)
{
    elem.begin_pos = begin_pos;
    name(elem.name);
    if (cur_char() == ':')
    {
        elem.ns = elem.name;
        next_check();
        name(elem.name);
    }
}

void parser_base::attribute_name(pstring& attr_ns, pstring& attr_name)
{
    name(attr_name);
    if (cur_char() == ':')
    {
        // Attribute name is namespaced.
        attr_ns = attr_name;
        next_check();
        name(attr_name);
    }
}

void parser_base::characters_with_encoded_char()
{
    assert(cur_char() == '&');
    parse_encoded_char();

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
}

}}
