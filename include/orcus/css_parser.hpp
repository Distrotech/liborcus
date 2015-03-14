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
class css_parser : public css::parser_base
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

    handler_type& m_handler;
};

template<typename _Handler>
css_parser<_Handler>::css_parser(const char* p, size_t n, handler_type& hdl) :
    css::parser_base(p, n), m_handler(hdl) {}

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
            case '#':
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
                os << "rule: failed to parse '" << c << "'";
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
        throw css::parse_error("at_rule_name: first character of an at-rule name must be an alphabet.");

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
    assert(has_char());
    char c = cur_char();
    if (c == '@')
    {
        // This is the name of an at-rule.
        at_rule_name();
        return;
    }

    assert(is_alpha(c) || c == '.' || c == '#');

    const char* p = NULL;
    size_t n = 0;

#if ORCUS_DEBUG_CSS
    cout << "selector_name:";
#endif

    if (c != '.' && c != '#')
    {
        identifier(p, n);
#if ORCUS_DEBUG_CSS
        std::string s(p, n);
        cout << " type=" << s;
#endif
        m_handler.simple_selector_type(p, n);
    }

    bool in_loop = true;
    while (in_loop && has_char())
    {
        switch (cur_char())
        {
            case '.':
            {
                next();
                identifier(p, n);
                m_handler.simple_selector_class(p, n);
#if ORCUS_DEBUG_CSS
                std::string s(p, n);
                std::cout << " class=" << s;
#endif
            }
            break;
            case '#':
            {
                next();
                identifier(p, n);
                m_handler.simple_selector_id(p, n);
#if ORCUS_DEBUG_CSS
                std::string s(p, n);
                std::cout << " id=" << s;
#endif
            }
            break;
            case ':':
            {
                // This could be either a pseudo element or pseudo class.
                next();
                if (cur_char() == ':')
                {
                    // pseudo element.
                    next();
                    identifier(p, n);
                    css::pseudo_element_t elem = css::to_pseudo_element(p, n);
                    if (!elem)
                    {
                        std::ostringstream os;
                        os << "selector_name: unknown pseudo element '";
                        write_to(os, p, n);
                        os << "'";
                        throw css::parse_error(os.str());
                    }

                    m_handler.simple_selector_pseudo_element(elem);
                }
                else
                {
                    // pseudo class (or pseudo element in the older version of CSS).
                    identifier(p, n);
                    m_handler.simple_selector_pseudo_class(p, n);
                }
            }
            break;
            default:
                in_loop = false;
        }
    }

    m_handler.end_simple_selector();
    skip_comments_and_blanks();

#if ORCUS_DEBUG_CSS
    std::cout << std::endl;
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
    skip_comments_and_blanks();

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
        throw css::parse_error("property: ':' expected.");
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
    const char* p = NULL;
    size_t len = 0;
    skip_to(p, len, '"');

    if (cur_char() != '"')
        throw css::parse_error("quoted_value: end quote has never been reached.");

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

    if (!is_alpha(c) && !is_numeric(c) && !is_in(c, "-+.#"))
    {
        std::ostringstream os;
        os << "value:: illegal first character of a value '" << c << "'";
        throw css::parse_error(os.str());
    }

    const char* p = NULL;
    size_t len = 0;
    identifier(p, len, ".%");
    m_handler.value(p, len);

    skip_comments_and_blanks();

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
        throw css::parse_error("block: '}' expected.");

    m_handler.end_block();

    next();
    skip_comments_and_blanks();

#if ORCUS_DEBUG_CSS
    std::cout << "}" << std::endl;
#endif
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
