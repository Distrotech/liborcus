/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_parser_base.hpp"
#include "orcus/global.hpp"
#include "orcus/cell_buffer.hpp"
#include "orcus/parser_global.hpp"

#include <mdds/sorted_string_map.hpp>

#include <limits>
#include <vector>
#include <deque>
#include <sstream>
#include <algorithm>

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

struct scope
{
    size_t width;
    scope_t type;

    scope(size_t _width) : width(_width), type(scope_t::unset) {}
};

struct parser_base::impl
{
    cell_buffer m_buffer;
    std::vector<scope> m_scopes;
    std::deque<pstring> m_line_buffer;
    const char* m_document;

    impl() : m_document(nullptr) {}
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

size_t parser_base::get_scope() const
{
    return (mp_impl->m_scopes.empty()) ? scope_empty : mp_impl->m_scopes.back().width;
}

void parser_base::push_scope(size_t scope_width)
{
    mp_impl->m_scopes.emplace_back(scope_width);
}

scope_t parser_base::get_scope_type() const
{
    assert(!mp_impl->m_scopes.empty());
    return mp_impl->m_scopes.back().type;
}

void parser_base::set_scope_type(scope_t type)
{
    assert(!mp_impl->m_scopes.empty());
    mp_impl->m_scopes.back().type = type;
}

size_t parser_base::pop_scope()
{
    assert(!mp_impl->m_scopes.empty());
    mp_impl->m_scopes.pop_back();
    return get_scope();
}

void parser_base::push_line_back(const char* p, size_t n)
{
    mp_impl->m_line_buffer.emplace_back(p, n);
}

pstring parser_base::pop_line_front()
{
    assert(!mp_impl->m_line_buffer.empty());

    pstring ret = mp_impl->m_line_buffer.front();
    mp_impl->m_line_buffer.pop_front();
    return ret;
}

bool parser_base::has_line_buffer() const
{
    return !mp_impl->m_line_buffer.empty();
}

size_t parser_base::get_line_buffer_count() const
{
    return mp_impl->m_line_buffer.size();
}

pstring parser_base::merge_line_buffer()
{
    assert(!mp_impl->m_line_buffer.empty());

    cell_buffer& buf = mp_impl->m_buffer;
    buf.reset();

    auto it = mp_impl->m_line_buffer.begin();
    buf.append(it->get(), it->size());
    ++it;

    std::for_each(it, mp_impl->m_line_buffer.end(),
        [&](const pstring& line)
        {
            buf.append(" ", 1);
            buf.append(line.get(), line.size());
        }
    );

    mp_impl->m_line_buffer.clear();

    return pstring(buf.get(), buf.size());
}

const char* parser_base::get_doc_hash() const
{
    return mp_impl->m_document;
}

void parser_base::set_doc_hash(const char* hash)
{
    mp_impl->m_document = hash;
}

namespace {

mdds::sorted_string_map<keyword_t>::entry keyword_entries[] = {
    { ORCUS_ASCII("FALSE"), keyword_t::boolean_false },
    { ORCUS_ASCII("False"), keyword_t::boolean_false },
    { ORCUS_ASCII("N"),     keyword_t::boolean_false },
    { ORCUS_ASCII("NO"),    keyword_t::boolean_false },
    { ORCUS_ASCII("NULL"),  keyword_t::null          },
    { ORCUS_ASCII("No"),    keyword_t::boolean_false },
    { ORCUS_ASCII("Null"),  keyword_t::null          },
    { ORCUS_ASCII("OFF"),   keyword_t::boolean_false },
    { ORCUS_ASCII("ON"),    keyword_t::boolean_true  },
    { ORCUS_ASCII("Off"),   keyword_t::boolean_false },
    { ORCUS_ASCII("On"),    keyword_t::boolean_true  },
    { ORCUS_ASCII("TRUE"),  keyword_t::boolean_true  },
    { ORCUS_ASCII("True"),  keyword_t::boolean_true  },
    { ORCUS_ASCII("Y"),     keyword_t::boolean_true  },
    { ORCUS_ASCII("YES"),   keyword_t::boolean_true  },
    { ORCUS_ASCII("Yes"),   keyword_t::boolean_true  },
    { ORCUS_ASCII("false"), keyword_t::boolean_false },
    { ORCUS_ASCII("n"),     keyword_t::boolean_false },
    { ORCUS_ASCII("no"),    keyword_t::boolean_false },
    { ORCUS_ASCII("null"),  keyword_t::null          },
    { ORCUS_ASCII("off"),   keyword_t::boolean_false },
    { ORCUS_ASCII("on"),    keyword_t::boolean_true  },
    { ORCUS_ASCII("true"),  keyword_t::boolean_true  },
    { ORCUS_ASCII("y"),     keyword_t::boolean_true  },
    { ORCUS_ASCII("yes"),   keyword_t::boolean_true  },
    { ORCUS_ASCII("~"),     keyword_t::null          },
};

}

keyword_t parser_base::parse_keyword(const char* p, size_t len)
{
    static mdds::sorted_string_map<keyword_t> map(
        keyword_entries,
        ORCUS_N_ELEMENTS(keyword_entries),
        keyword_t::unknown);

    keyword_t value = map.find(p, len);
    return value;
}

pstring parser_base::parse_quoted_string_value(const char*& p, size_t max_length)
{
    parse_quoted_string_state ret =
        parse_quoted_string(p, max_length, mp_impl->m_buffer);

    if (!ret.str)
    {
        std::ostringstream os;
        os << "parse_quoted_string_value: failed to parse ";
        switch (ret.length)
        {
            case parse_quoted_string_state::error_illegal_escape_char:
                os << " due to the presence of illegal escape character.";
            break;
            case parse_quoted_string_state::error_no_closing_quote:
                os << "because the closing quote was not found.";
            break;
            default:
                os << " due to unknown reason.";

        }
        throw parse_error(os.str());
    }

    return pstring(ret.str, ret.length);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
