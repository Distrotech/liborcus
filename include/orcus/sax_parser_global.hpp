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

#ifndef ORCUS_SAX_PARSER_GLOBAL_HPP
#define ORCUS_SAX_PARSER_GLOBAL_HPP

#include "env.hpp"
#include "pstring.hpp"
#include "cell_buffer.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <sstream>

#define ORCUS_DEBUG_SAX_PARSER 0

#if ORCUS_DEBUG_SAX_PARSER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus { namespace sax {

class ORCUS_DLLPUBLIC malformed_xml_error : public std::exception
{
    std::string m_msg;
public:
    malformed_xml_error(const std::string& msg);
    virtual ~malformed_xml_error() throw();
    virtual const char* what() const throw();
};

/**
 * Document type declaration passed by sax_parser to its handler's doctype()
 * call.
 */
struct doctype_declaration
{
    enum keyword_type { keyword_public, keyword_private };

    keyword_type keyword;
    pstring root_element;
    pstring fpi;
    pstring uri;
};

/**
 * Given an encoded name (such as 'quot' and 'amp'), return a single
 * character that corresponds with the name.  The name shouldn't include the
 * leading '&' and trailing ';'.
 *
 * @param p pointer to the first character of encoded name
 * @param n length of encoded name
 *
 * @return single character that corresponds with the encoded name.  '\0' is
 *         returned if decoding fails.
 */
ORCUS_DLLPUBLIC char decode_xml_encoded_char(const char* p, size_t n);

ORCUS_DLLPUBLIC bool is_blank(char c);
ORCUS_DLLPUBLIC bool is_alpha(char c);
ORCUS_DLLPUBLIC bool is_name_char(char c);
ORCUS_DLLPUBLIC bool is_numeric(char c);

/**
 * Element properties passed by sax_parser to its handler's open_element()
 * and close_element() calls.
 */
struct parser_element
{
    pstring ns;            // element namespace (optional)
    pstring name;          // element name
    const char* begin_pos; // position of the opening brace '<'.
    const char* end_pos;   // position of the char after the closing brace '>'.
};

class ORCUS_DLLPUBLIC parser_base
{
protected:
    cell_buffer m_cell_buf;
    const char* m_content;
    const char* m_char;
    const size_t m_size;
    size_t m_pos;
    size_t m_nest_level;
    bool m_root_elem_open:1;

public:
    parser_base(const char* content, size_t size);

protected:
    void next() { ++m_pos; ++m_char; }

    void next_check()
    {
        next();
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.");
    }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        assert(m_nest_level > 0);
        --m_nest_level;
    }

    bool has_char() const { return m_pos < m_size; }

    void has_char_throw(const char* msg) const
    {
        if (!has_char())
            throw malformed_xml_error(msg);
    }

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

    char cur_char_checked() const
    {
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.");

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

    char next_char_checked()
    {
        next();
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.");

        return *m_char;
    }

    void blank();
    void comment();

    /**
     * Skip an optional byte order mark at the begining of the xml stream.
     */
    void skip_bom();

    void expects_next(const char* p, size_t n);

    void parse_encoded_char();
    void value_with_encoded_char(pstring& str);

    /**
     * Parse quoted value.  Note that the retreived string may be stored in
     * the temporary cell buffer if the decode parameter is true. Use the
     * string immediately after this call before the buffer becomes invalid.
     */
    void value(pstring& str, bool decode);

    void name(pstring& str);
    void element_name(parser_element& elem);
};

}}

#endif
