/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_YAML_PARSER_HPP
#define INCLUDED_ORCUS_YAML_PARSER_HPP

#include "orcus/yaml_parser_base.hpp"

#include <iostream>

namespace orcus {

template<typename _Handler>
class yaml_parser : public yaml::parser_base
{
public:
    typedef _Handler handler_type;

    yaml_parser(const char* p, size_t n, handler_type& hdl);

    void parse();

private:
    void parse_line(const pstring& line);

private:
    handler_type& m_handler;
};

template<typename _Handler>
yaml_parser<_Handler>::yaml_parser(const char* p, size_t n, handler_type& hdl) :
    yaml::parser_base(p, n), m_handler(hdl) {}

template<typename _Handler>
void yaml_parser<_Handler>::parse()
{
    while (has_char())
    {
        size_t indent = parse_indent();
        if (indent == parse_indent_end_of_stream)
            return;

        if (indent == parse_indent_blank_line)
            continue;

        size_t cur_scope = get_current_scope();
        if (cur_scope == scope_empty || indent > cur_scope)
        {
            std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): push " << indent << std::endl;
            push_scope(indent);
        }
        else if (indent < cur_scope)
        {
            // Current indent is less than the current scope level.
            do
            {
                cur_scope = pop_scope();
                std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): scope after pop " << cur_scope << std::endl;
                if (cur_scope < indent)
                    throw yaml::parse_error("parse: invalid indent level.");
            }
            while (indent < cur_scope);
        }

        // Parse the rest of the line.
        pstring line = parse_to_end_of_line();

        assert(!line.empty());

        std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): indent: " << indent << std::endl;
        std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): line='" << line << "'" << std::endl;

        parse_line(line);
    }
}

template<typename _Handler>
void yaml_parser<_Handler>::parse_line(const pstring& line)
{
    const char* p = line.get();
    const char* p_end = p + line.size();

    if (*p == '-')
    {
        ++p;
        if (p == p_end)
        {
            // List item start.
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

                std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse_line): start of document" << std::endl;
            }
            break;
            case ' ':
            {
                // list item start with inline first item content.
                ++p;
                if (p == p_end)
                    throw yaml::parse_error("parse_line: list item expected, but the line ended prematurely.");

                // Skip all white spaces.
                size_t n = 1;
                for (; *p == ' '; ++p, ++n)
                    ;

                std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse_line): n = " << n << std::endl;
            }
            break;
        }
    }
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
