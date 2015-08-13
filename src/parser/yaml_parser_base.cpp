/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_parser_base.hpp"
#include "orcus/global.hpp"

#include <limits>
#include <vector>

namespace orcus { namespace yaml {

parse_error::parse_error(const std::string& msg) : ::orcus::parse_error(msg) {}

void parse_error::throw_with(const char* msg_before, char c, const char* msg_after)
{
    throw parse_error(build_message(msg_before, c, msg_after));
}

void parse_error::throw_with(
    const char* msg_before, const char* p, size_t n, const char* msg_after)
{
    throw parse_error(build_message(msg_before, p, n, msg_after));
}

struct parser_base::impl
{
    std::vector<size_t> m_scopes;
};

const size_t parser_base::parse_indent_blank_line    = std::numeric_limits<size_t>::max();
const size_t parser_base::parse_indent_end_of_stream = std::numeric_limits<size_t>::max() - 1;
const size_t parser_base::scope_empty = std::numeric_limits<size_t>::max() - 2;

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(make_unique<impl>()) {}

parser_base::~parser_base() {}

size_t parser_base::parse_indent()
{
    for (size_t indent = 0; has_char(); next(), ++indent)
    {
        char c = cur_char();
        switch (c)
        {
            case '#':
                skip_comment();
                return parse_indent_blank_line;
            case '\n':
                next();
                return parse_indent_blank_line;
            case ' ':
                continue;
            default:
                return indent;
        }
    }

    return parse_indent_end_of_stream;
}

pstring parser_base::parse_to_end_of_line()
{
    assert(cur_char() != ' ');
    const char* p = mp_char;
    size_t len = 0;
    for (; has_char(); next(), ++len)
    {
        switch (cur_char())
        {
            case '#':
                skip_comment();
            break;
            case '\n':
                next();
            break;
            default:
                continue;
        }
        break;
    }

    pstring ret(p, len);
    ret = ret.trim();
    return ret;
}

void parser_base::skip_comment()
{
    assert(cur_char() == '#');

    for (; has_char(); next())
    {
        if (cur_char() == '\n')
        {
            next();
            break;
        }
    }
}

size_t parser_base::get_current_scope() const
{
    return (mp_impl->m_scopes.empty()) ? scope_empty : mp_impl->m_scopes.back();
}

void parser_base::push_scope(size_t scope_width)
{
    mp_impl->m_scopes.push_back(scope_width);
}

size_t parser_base::pop_scope()
{
    assert(!mp_impl->m_scopes.empty());
    mp_impl->m_scopes.pop_back();
    return get_current_scope();
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
