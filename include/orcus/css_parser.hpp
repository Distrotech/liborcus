/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CSS_PARSER_HPP
#define INCLUDED_ORCUS_CSS_PARSER_HPP

#define ORCUS_DEBUG_CSS 0

#include "parser_global.hpp"
#include "css_parser_base.hpp"

#include <cassert>
#include <sstream>

#if ORCUS_DEBUG_CSS
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus {

template<typename _Handler>
class css_parser
{
public:
    typedef _Handler handler_type;

    css_parser(const char* p, size_t n, handler_type& hdl);
    void parse();

private:
    // Handlers - at the time a handler is called the current position is
    // expected to point to the first unprocessed non-blank character, and
    // each handler must set the current position to the next unprocessed
    // non-blank character when it finishes.
    void rule();
    void at_rule_name();
    void selector_name();
    void property_name();
    void property();
    void quoted_value();
    void value();
    void name_sep();
    void property_sep();
    void block();

    bool skip_comment();
    void comment();
    void skip_comments_and_blanks();

    void identifier(const char*& p, size_t& len);

    void skip_blanks();
    void skip_blanks_reverse();
    void shrink_stream();
    void next();
    char cur_char() const;

    /**
     * The caller must ensure that the next character exists.
     */
    char next_char() const;

    size_t remaining_size() const { return m_length - m_pos - 1; }
    bool has_char() const { return m_pos < m_length; }

    handler_type& m_handler;
    const char* mp_char;
    size_t m_pos;
    size_t m_length;
};

template<typename _Handler>
css_parser<_Handler>::css_parser(const char* p, size_t n, handler_type& hdl) :
    m_handler(hdl), mp_char(p), m_pos(0), m_length(n) {}

template<typename _Handler>
void css_parser<_Handler>::parse()
{
    shrink_stream();

#if ORCUS_DEBUG_CSS
    std::cout << "compressed: '";
    const char* p = mp_char;
    for (size_t i = m_pos; i < m_length; ++i, ++p)
        std::cout << *p;
    std::cout << "'" << std::endl;
#endif
    m_handler.begin_parse();
    while (has_char())
        rule();
    m_handler.end_parse();
}

template<typename _Handler>
void css_parser<_Handler>::rule()
{
    // <selector name> , ... , <selector name> <block>
    while (has_char())
    {
        if (skip_comment())
            continue;

        char c = cur_char();

        if (is_alpha(c))
        {
            selector_name();
            continue;
        }

        switch (c)
        {
            case '.':
            case '@':
                selector_name();
            break;
            case ',':
                name_sep();
            break;
            case '{':
                block();
            break;
            default:
            {
                std::ostringstream os;
                os << "failed to parse '" << c << "'";
                throw css::parse_error(os.str());
            }
        }
    }
}

template<typename _Handler>
void css_parser<_Handler>::at_rule_name()
{
    assert(has_char());
    assert(cur_char() == '@');
    next();
    char c = cur_char();
    if (!is_alpha(c))
        throw css::parse_error("first character of an at-rule name must be an alphabet.");

    const char* p;
    size_t len;
    identifier(p, len);
    skip_blanks();

    m_handler.at_rule_name(p, len);
#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "at-rule name: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::selector_name()
{
    // <element name>
    // '.' <class name>
    // <element name> '.' <class name>
    //
    // Both element and class names are identifiers.

    assert(has_char());
    char c = cur_char();
    if (c == '@')
    {
        // This is the name of an at-rule.
        at_rule_name();
        return;
    }

    if (!is_alpha(c) && c != '.')
    {
        std::ostringstream os;
        os << "selector_name: first character of a name must be an alphabet or a dot, but found '" << c << "'";
        throw css::parse_error(os.str());
    }

    const char* p_elem = NULL;
    const char* p_class = NULL;
    size_t len_elem = 0;
    size_t len_class = 0;
    if (c != '.')
        identifier(p_elem, len_elem);

    if (cur_char() == '.')
    {
        next();
        identifier(p_class, len_class);
    }

    skip_comments_and_blanks();

    m_handler.simple_selector(p_elem, len_elem, p_class, len_class);
#if ORCUS_DEBUG_CSS
    std::string elem_name(p_elem, len_elem), class_name(p_class, len_class);
    std::cout << "selector name: (element)'" << elem_name.c_str() << "' (class)'" << class_name.c_str() << "'" << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::property_name()
{
    // <identifier>

    assert(has_char());
    char c = cur_char();
    if (!is_alpha(c) && c != '.')
    {
        std::ostringstream os;
        os << "property_name: first character of a name must be an alphabet or a dot, but found '" << c << "'";
        throw css::parse_error(os.str());
    }

    const char* p;
    size_t len;
    identifier(p, len);
    skip_blanks();

    m_handler.property_name(p, len);
#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "property name: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::property()
{
    // <property name> : <value> , ... , <value>

    m_handler.begin_property();
    property_name();
    if (cur_char() != ':')
        throw css::parse_error("':' expected.");
    next();
    skip_comments_and_blanks();

    bool in_loop = true;
    while (in_loop && has_char())
    {
        value();
        char c = cur_char();
        switch (c)
        {
            case ',':
            {
                // separated by commas.
                next();
                skip_comments_and_blanks();
            }
            break;
            case ';':
            case '}':
                in_loop = false;
            break;
            default:
                ;
        }
    }

    skip_comments_and_blanks();
    m_handler.end_property();
}

template<typename _Handler>
void css_parser<_Handler>::quoted_value()
{
    // Parse until the the end quote is reached.

    assert(cur_char() == '"');
    next();
    const char* p = mp_char;
    size_t len = 1;
    for (next(); has_char(); next())
    {
        if (cur_char() == '"')
        {
            // End quote reached.
            break;
        }
        ++len;
    }

    if (cur_char() != '"')
        throw css::parse_error("end quote has never been reached.");

    next();
    skip_blanks();

    m_handler.value(p, len);
#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "quoted value: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::value()
{
    assert(has_char());
    char c = cur_char();
    if (c == '"')
    {
        quoted_value();
        return;
    }

    if (!is_alpha(c) && !is_numeric(c) && c != '-' && c != '+' && c != '.')
    {
        std::ostringstream os;
        os << "value:: illegal first character of a value '" << c << "'";
        throw css::parse_error(os.str());
    }

    const char* p = mp_char;
    size_t len = 1;
    for (next(); has_char(); next(), ++len)
    {
        c = cur_char();

        if (is_alpha(c) || is_name_char(c) || is_numeric(c))
            continue;

        switch (c)
        {
            case '.':
            case '%':
                // These characters are allowed in a property value.
                continue;
            default:
                ;
        }

        break;
    }
    skip_blanks();

    m_handler.value(p, len);
#if ORCUS_DEBUG_CSS
    std::string foo(p, len);
    std::cout << "value: " << foo.c_str() << std::endl;
#endif
}

template<typename _Handler>
void css_parser<_Handler>::name_sep()
{
    assert(cur_char() == ',');
#if ORCUS_DEBUG_CSS
    std::cout << "," << std::endl;
#endif
    next();
    skip_blanks();
    m_handler.end_selector();
}

template<typename _Handler>
void css_parser<_Handler>::property_sep()
{
#if ORCUS_DEBUG_CSS
    std::cout << ";" << std::endl;
#endif
    next();
    skip_comments_and_blanks();
}

template<typename _Handler>
void css_parser<_Handler>::block()
{
    // '{' <property> ';' ... ';' <property> ';'(optional) '}'

    assert(cur_char() == '{');
#if ORCUS_DEBUG_CSS
    std::cout << "{" << std::endl;
#endif
    m_handler.end_selector();
    m_handler.begin_block();

    next();
    skip_comments_and_blanks();

    // parse properties.
    while (has_char())
    {
        property();
        if (cur_char() != ';')
            break;
        property_sep();
        if (cur_char() == '}')
            // ';' after the last property.  This is optional but allowed.
            break;
    }

    if (cur_char() != '}')
        throw css::parse_error("} expected.");

    m_handler.end_block();

    next();
    skip_comments_and_blanks();

#if ORCUS_DEBUG_CSS
    std::cout << "}" << std::endl;
#endif
}

template<typename _Handler>
bool css_parser<_Handler>::skip_comment()
{
    char c = cur_char();
    if (c != '/')
        return false;

    if (remaining_size() > 2 && next_char() == '*')
    {
        next();
        comment();
        skip_blanks();
        return true;
    }

    return false;
}

template<typename _Handler>
void css_parser<_Handler>::comment()
{
    assert(cur_char() == '*');

    // Parse until we reach either EOF or '*/'.
    bool has_star = false;
    for (next(); has_char(); next())
    {
        char c = cur_char();
        if (has_star && c == '/')
        {
            next();
            return;
        }
        has_star = (c == '*');
    }

    // EOF reached.
}

template<typename _Handler>
void css_parser<_Handler>::skip_comments_and_blanks()
{
    skip_blanks();
    while (skip_comment())
        ;
}

template<typename _Handler>
void css_parser<_Handler>::identifier(const char*& p, size_t& len)
{
    p = mp_char;
    len = 1;
    for (next(); has_char(); next())
    {
        char c = cur_char();
        if (!is_alpha(c) && !is_name_char(c) && !is_numeric(c))
            break;
        ++len;
    }
}

template<typename _Handler>
void css_parser<_Handler>::skip_blanks()
{
    for (; has_char(); next())
    {
        if (!is_blank(*mp_char))
            break;
    }
}

template<typename _Handler>
void css_parser<_Handler>::skip_blanks_reverse()
{
    const char* p = mp_char + remaining_size();
    for (; p != mp_char; --p, --m_length)
    {
        if (!is_blank(*p))
            break;
    }
}

template<typename _Handler>
void css_parser<_Handler>::shrink_stream()
{
    // Skip any leading blanks.
    skip_blanks();

    if (!remaining_size())
        return;

    // Skip any trailing blanks.
    skip_blanks_reverse();

    // Skip leading <!-- if present.

    const char* com_open = "<!--";
    size_t com_open_len = std::strlen(com_open);
    if (remaining_size() < com_open_len)
        // Not enough stream left.  Bail out.
        return;

    const char* p = mp_char;
    for (size_t i = 0; i < com_open_len; ++i, ++p)
    {
        if (*p != com_open[i])
            return;
        next();
    }
    mp_char = p;

    // Skip leading blanks once again.
    skip_blanks();

    // Skip trailing --> if present.
    const char* com_close = "-->";
    size_t com_close_len = std::strlen(com_close);
    size_t n = remaining_size();
    if (n < com_close_len)
        // Not enough stream left.  Bail out.
        return;

    p = mp_char + n; // move to the last char.
    for (size_t i = com_close_len; i > 0; --i, --p)
    {
        if (*p != com_close[i-1])
            return;
    }
    m_length -= com_close_len;

    skip_blanks_reverse();
}

template<typename _Handler>
void css_parser<_Handler>::next()
{
    ++m_pos;
    ++mp_char;
}

template<typename _Handler>
char css_parser<_Handler>::cur_char() const
{
    return *mp_char;
}

template<typename _Handler>
char css_parser<_Handler>::next_char() const
{
    const char* p = mp_char;
    ++p;
    return *p;
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
