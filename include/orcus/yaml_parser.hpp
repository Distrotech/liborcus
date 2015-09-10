/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_YAML_PARSER_HPP
#define INCLUDED_ORCUS_YAML_PARSER_HPP

#include "orcus/yaml_parser_base.hpp"
#include "orcus/parser_global.hpp"

namespace orcus {

template<typename _Handler>
class yaml_parser : public yaml::parser_base
{
public:
    typedef _Handler handler_type;

    yaml_parser(const char* p, size_t n, handler_type& hdl);

    void parse();

private:
    size_t end_scope();
    void check_or_begin_document();
    void check_or_begin_map();
    void check_or_begin_sequence();
    void parse_value(const char* p, size_t len);
    void push_value(const char* p, size_t len);
    void parse_line(const char* p, size_t len);
    void parse_map_key(const char* p, size_t len);

private:
    handler_type& m_handler;
};

template<typename _Handler>
yaml_parser<_Handler>::yaml_parser(const char* p, size_t n, handler_type& hdl) :
    yaml::parser_base(p, n), m_handler(hdl) {}

template<typename _Handler>
void yaml_parser<_Handler>::parse()
{
    m_handler.begin_parse();

    while (has_char())
    {
        size_t indent = parse_indent();
        if (indent == parse_indent_end_of_stream)
            break;

        if (indent == parse_indent_blank_line)
            continue;

        size_t cur_scope = get_scope();

        if (has_line_buffer() && indent >= cur_scope)
        {
            // This line is part of multi-line string.  Push the line to the
            // buffer as-is.
            if (get_scope_type() != yaml::scope_t::multi_line_string)
                set_scope_type(yaml::scope_t::multi_line_string);

            pstring line = parse_to_end_of_line();
            assert(!line.empty());
            push_line_back(line.get(), line.size());
            continue;
        }

        if (cur_scope == scope_empty || indent > cur_scope)
        {
            push_scope(indent);
        }
        else if (indent < cur_scope)
        {
            // Current indent is less than the current scope level.
            do
            {
                cur_scope = end_scope();
                if (cur_scope < indent)
                    throw yaml::parse_error("parse: invalid indent level.");
            }
            while (indent < cur_scope);
        }

        // Parse the rest of the line.
        pstring line = parse_to_end_of_line();

        assert(!line.empty());
        parse_line(line.get(), line.size());
    }

    // End all remaining scopes.
    size_t cur_scope = get_scope();
    while (cur_scope != scope_empty)
        cur_scope = end_scope();

    if (get_doc_hash())
        m_handler.end_document();

    m_handler.end_parse();
}

template<typename _Handler>
size_t yaml_parser<_Handler>::end_scope()
{
    switch (get_scope_type())
    {
        case yaml::scope_t::map:
            m_handler.end_map();
        break;
        case yaml::scope_t::sequence:
            m_handler.end_sequence();
        break;
        case yaml::scope_t::multi_line_string:
        {
            pstring merged = merge_line_buffer();
            m_handler.string(merged.get(), merged.size());
        }
        break;
        default:
        {
            if (has_line_buffer())
            {
                assert(get_line_buffer_count() == 1);
                pstring line = pop_line_front();
                parse_value(line.get(), line.size());
            }
        }
    }
    return pop_scope();
}

template<typename _Handler>
void yaml_parser<_Handler>::check_or_begin_document()
{
    if (!get_doc_hash())
    {
        set_doc_hash(mp_char);
        m_handler.begin_document();
    }
}

template<typename _Handler>
void yaml_parser<_Handler>::check_or_begin_map()
{
    if (get_scope_type() == yaml::scope_t::unset)
    {
        check_or_begin_document();
        set_scope_type(yaml::scope_t::map);
        m_handler.begin_map();
    }
}

template<typename _Handler>
void yaml_parser<_Handler>::check_or_begin_sequence()
{
    if (get_scope_type() == yaml::scope_t::unset)
    {
        check_or_begin_document();
        set_scope_type(yaml::scope_t::sequence);
        m_handler.begin_sequence();
    }
}

template<typename _Handler>
void yaml_parser<_Handler>::parse_value(const char* p, size_t len)
{
    check_or_begin_document();

    const char* p0 = p;
    const char* p_end = p + len;
    double val = parse_numeric(p, len);
    if (p == p_end)
    {
        m_handler.number(val);
        return;
    }

    yaml::keyword_t kw = parse_keyword(p0, len);

    if (kw != yaml::keyword_t::unknown)
    {
        switch (kw)
        {
            case yaml::keyword_t::null:
                m_handler.null();
            break;
            case yaml::keyword_t::boolean_true:
                m_handler.boolean_true();
            break;
            case yaml::keyword_t::boolean_false:
                m_handler.boolean_false();
            break;
            default:
                ;
        }

        return;
    }

    // Failed to parse it as a number or a keyword.  It must be a string.
    m_handler.string(p0, len);
}

template<typename _Handler>
void yaml_parser<_Handler>::push_value(const char* p, size_t len)
{
    if (has_line_buffer() && get_scope_type() == yaml::scope_t::unset)
        set_scope_type(yaml::scope_t::multi_line_string);

    push_line_back(p, len);
}

template<typename _Handler>
void yaml_parser<_Handler>::parse_line(const char* p, size_t len)
{
    const char* p_end = p + len;

    if (*p == '-')
    {
        ++p;
        if (p == p_end)
        {
            // List item start.
            check_or_begin_sequence();
            return;
        }

        switch (*p)
        {
            case '-':
            {
                // start of a document
                ++p;
                if (p == p_end)
                    throw yaml::parse_error("parse_line: line ended with '--'.");

                if (*p != '-')
                    yaml::parse_error::throw_with("parse_line: '-' expected but '", *p, "' found.");

                ++p; // Skip the '-'.
                set_doc_hash(p);
                m_handler.begin_document();

                if (p != p_end)
                {
                    // Skip all white spaces.
                    for (++p; *p == ' '; ++p)
                        ;

                    // Whatever comes after '---' is equivalent of first node.
                    assert(p != p_end);
                    push_scope(0);
                    parse_line(p, p_end-p);
                }
            }
            break;
            case ' ':
            {
                check_or_begin_sequence();

                // list item start with inline first item content.
                ++p;
                if (p == p_end)
                    throw yaml::parse_error("parse_line: list item expected, but the line ended prematurely.");

                // Skip all white spaces.
                size_t n = 1;
                for (; *p == ' '; ++p, ++n)
                    ;

                size_t scope_width = get_scope() + 1 + n;
                push_scope(scope_width);
                parse_line(p, p_end-p);
            }
            break;
        }

        return;
    }

    // If the line doesn't start with a '-', it must be a dictionary key.
    parse_map_key(p, len);
}

template<typename _Handler>
void yaml_parser<_Handler>::parse_map_key(const char* p, size_t len)
{
    const char* p_end = p + len;
    size_t n = 0;

    if (*p == '"')
    {
        pstring quoted_str = parse_quoted_string_value(p, len);

        if (p == p_end)
        {
            m_handler.string(quoted_str.get(), quoted_str.size());
            return;
        }

        // Skip all white spaces.
        for (; p != p_end && *p == ' '; ++p)
            ;

        if (*p != ':')
            throw yaml::parse_error("parse_map_key: ':' is expected after the quoted string key.");

        check_or_begin_map();
        m_handler.begin_map_key();
        m_handler.string(quoted_str.get(), quoted_str.size());
        m_handler.end_map_key();
    }
    else
    {
        const char* p0 = p;  // Save the original head position.
        bool has_key = false;
        const char* p_last_non_blank = nullptr;
        for (; p != p_end; ++p)
        {
            if (*p == ':')
            {
                has_key = true;
                break;
            }

            if (*p != ' ')
                p_last_non_blank = p;
        }

        if (!has_key)
        {
            // No map key found.
            push_value(p0, len);
            return;
        }

        check_or_begin_map();
        m_handler.begin_map_key();
        n = p_last_non_blank - p0 + 1;
        parse_value(p0, n);
        m_handler.end_map_key();
    }

    ++p;  // skip the ':'.
    if (p == p_end)
        return;

    // Skip all white spaces.
    n = 1;
    for (; *p == ' '; ++p, ++n)
        ;

    size_t scope_width = get_scope() + 1 + n;
    push_scope(scope_width);

    // inline map item.
    if (*p == '-')
        throw yaml::parse_error("parse_map_key: sequence entry is not allowed as an inline map item.");

    if (*p == '"')
    {
        pstring quoted_str = parse_quoted_string_value(p, p_end-p);
        if (p != p_end)
            throw yaml::parse_error("parse_map_key: unexpected trailing string segment after the quoted string.");

        m_handler.string(quoted_str.get(), quoted_str.size());
    }
    else
    {
        n = 0;
        const char* p0 = p;
        for (; p != p_end; ++p, ++n)
        {
            if (*p == ':')
                throw yaml::parse_error("parse_map_key: nested inline map key is not allowed.");
        }

        push_value(p0, n);
    }
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
