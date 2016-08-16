/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SAX_PARSER_BASE_HPP
#define INCLUDED_ORCUS_SAX_PARSER_BASE_HPP

#include "env.hpp"
#include "pstring.hpp"
#include "cell_buffer.hpp"
#include "parser_global.hpp"
#include "parser_base.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <memory>

#define ORCUS_DEBUG_SAX_PARSER 0

#if ORCUS_DEBUG_SAX_PARSER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus { namespace sax {

class ORCUS_PSR_DLLPUBLIC malformed_xml_error : public ::orcus::parse_error
{
public:
    malformed_xml_error() = delete;
    malformed_xml_error(const std::string& msg, std::ptrdiff_t offset);
    virtual ~malformed_xml_error() throw();
};

/**
 * Document type declaration passed by sax_parser to its handler's doctype()
 * call.
 */
struct doctype_declaration
{
    enum class keyword_type { dtd_public, dtd_private };

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
ORCUS_PSR_DLLPUBLIC char decode_xml_encoded_char(const char* p, size_t n);

/**
 * Given an unicode string (such as #20A9 ), return a std::string
 * that corresponds with the name.  The name shouldn't include the
 * leading '&' and trailing ';'.
 *
 * @param p pointer to the first character of encoded name
 * @param n length of encoded name
 *
 * @return string that corresponds with the encoded name.  empty string is
 *         returned if decoding fails.
 */
ORCUS_PSR_DLLPUBLIC std::string decode_xml_unicode_char(const char* p, size_t n);

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

/**
 * Attribute properties passed by sax_parser to its handler's attribute()
 * call. When an attribute value is transient, it has been converted due to
 * presence of encoded character(s) and stored in a temporary buffer. The
 * handler must assume that the value will not survive beyond the scope of
 * the callback.
 */
struct parser_attribute
{
    pstring ns;      // attribute namespace (optional)
    pstring name;    // attribute name
    pstring value;   // attribute value
    bool transient;  // whether or not the attribute value is on a temporary buffer.
};

class ORCUS_PSR_DLLPUBLIC parser_base : public ::orcus::parser_base
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

    parser_base() = delete;
    parser_base(const parser_base&) = delete;
    parser_base& operator=(const parser_base&) = delete;
protected:
    size_t m_nest_level;
    size_t m_buffer_pos;
    bool m_root_elem_open:1;

protected:
    parser_base(const char* content, size_t size);
    ~parser_base();

    void next_check()
    {
        next();
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.", offset());
    }

    void nest_up() { ++m_nest_level; }
    void nest_down()
    {
        assert(m_nest_level > 0);
        --m_nest_level;
    }

    void inc_buffer_pos();
    void reset_buffer_pos() { m_buffer_pos = 0; }

    void has_char_throw(const char* msg) const
    {
        if (!has_char())
            throw malformed_xml_error(msg, offset());
    }

    /**
     * Determine the number of remaining characters <strong>including</strong>
     * the current character.
     *
     *
     * @return number of remaining characters including the current character.
     */
    inline size_t remains() const
    {
#if ORCUS_DEBUG_SAX_PARSER
        if (mp_char >= mp_end)
            throw malformed_xml_error("xml stream ended prematurely.", offset());
#endif
        return mp_end - mp_char;
    }

    char cur_char_checked() const
    {
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.", offset());

        return *mp_char;
    }

    char next_and_char()
    {
        next();
#if ORCUS_DEBUG_SAX_PARSER
        if (mp_char >= mp_end)
            throw malformed_xml_error("xml stream ended prematurely.", offset());
#endif
        return *mp_char;
    }

    char next_char_checked()
    {
        next();
        if (!has_char())
            throw malformed_xml_error("xml stream ended prematurely.", offset());

        return *mp_char;
    }

    cell_buffer& get_cell_buffer();

    void blank();
    void comment();

    /**
     * Skip an optional byte order mark at the begining of the xml stream.
     */
    void skip_bom();

    void expects_next(const char* p, size_t n);

    void parse_encoded_char(cell_buffer& buf);
    void value_with_encoded_char(cell_buffer& buf, pstring& str);

    /**
     * Parse quoted value.  Note that the retrieved string may be stored in
     * the temporary cell buffer if the decode parameter is true. Use the
     * string immediately after this call before the buffer becomes invalid.
     *
     * @return true if the value is stored in temporary buffer, false
     *         otherwise.
     */
    bool value(pstring& str, bool decode);

    void name(pstring& str);
    void element_name(parser_element& elem, const char* begin_pos);
    void attribute_name(pstring& attr_ns, pstring& attr_name);
    void characters_with_encoded_char(cell_buffer& buf);
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
