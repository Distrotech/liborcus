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

        // Parse the rest of the line.
        pstring line = parse_to_end_of_line();

        std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): indent: " << indent << std::endl;
        std::cout << __FILE__ << "#" << __LINE__ << " (yaml_parser:parse): line='" << line << "'" << std::endl;
    }
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
