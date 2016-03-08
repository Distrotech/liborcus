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

parse_error::parse_error(const std::string& msg) :
    ::orcus::parse_error(msg, 0) {}

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

    bool m_in_literal_block;

    impl() : m_document(nullptr), m_in_literal_block(false) {}
};

const size_t parser_base::parse_indent_blank_line    = std::numeric_limits<size_t>::max();
const size_t parser_base::parse_indent_end_of_stream = std::numeric_limits<size_t>::max() - 1;
const size_t parser_base::scope_empty = std::numeric_limits<size_t>::max() - 2;

parser_base::parser_base(const char* p, size_t n) :
    ::orcus::parser_base(p, n), mp_impl(orcus::make_unique<impl>()) {}

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
    const char* p = mp_char;
    size_t len = 0;
    for (; has_char(); next(), ++len)
    {
        switch (cur_char())
        {
            case '#':
                skip_comment();
            break;
            case '\'':
            {
                const char* p_open_quote = mp_char;

                // character immediately after the closing quote.
                const char* p_end =
                    parse_to_closing_single_quote(mp_char, remaining_size());

                if (!p_end)
                    throw parse_error("parse_to_end_of_line: closing single quote was expected but not found.");

                size_t diff = p_end - p_open_quote - 1;

                // Move the cursor to the closing quote.
                next(diff);
                len += diff;
                assert(cur_char() == '\'');
                continue;
            }
            break;
            case '"':
            {
                const char* p_open_quote = mp_char;

                // character immediately after the closing quote.
                const char* p_end =
                    parse_to_closing_double_quote(mp_char, remaining_size());

                if (!p_end)
                    throw parse_error("parse_to_end_of_line: closing double quote was expected but not found.");

                size_t diff = p_end - p_open_quote - 1;

                // Move the cursor to the closing quote.
                next(diff);
                len += diff;
                assert(cur_char() == '"');
                continue;
            }
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

    char sep = mp_impl->m_in_literal_block ? '\n' : ' ';

    cell_buffer& buf = mp_impl->m_buffer;
    buf.reset();

    auto it = mp_impl->m_line_buffer.begin();
    buf.append(it->get(), it->size());
    ++it;

    std::for_each(it, mp_impl->m_line_buffer.end(),
        [&](const pstring& line)
        {
            buf.append(&sep, 1);
            buf.append(line.get(), line.size());
        }
    );

    mp_impl->m_line_buffer.clear();
    mp_impl->m_in_literal_block = false;

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

void throw_quoted_string_parse_error(const char* func_name, const parse_quoted_string_state& ret)
{
    std::ostringstream os;
    os << func_name << ": failed to parse ";
    if (ret.length == parse_quoted_string_state::error_illegal_escape_char)
        os << "due to the presence of illegal escape character.";
    else if (ret.length == parse_quoted_string_state::error_no_closing_quote)
        os << "because the closing quote was not found.";
    else
        os << "due to unknown reason.";

    throw parse_error(os.str());
}

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

parser_base::key_value parser_base::parse_key_value(const char* p, size_t len)
{
    assert(*p != ' ');
    assert(len);

    const char* p_end = p + len;

    key_value kv;

    char last = 0;
    bool in_key = true;

    const char* p_head = p;

    for (; p != p_end; ++p)
    {
        if (*p == ' ')
        {
            if (in_key)
            {
                if (last == ':')
                {
                    // Key found.
                    kv.key = pstring(p_head, p-p_head-1).trim();
                    in_key = false;
                    p_head = nullptr;
                }
            }
        }
        else
        {
            if (!p_head)
                p_head = p;
        }

        last = *p;
    }

    assert(p_head);

    if (in_key && last == ':')
    {
        // Line only contains a key and ends with ':'.
        kv.key = pstring(p_head, p-p_head-1).trim();
    }
    else
    {
        // Key has already been found and the value comes after the ':'.
        kv.value = pstring(p_head, p-p_head);
    }

    return kv;
}

pstring parser_base::parse_single_quoted_string_value(const char*& p, size_t max_length)
{
    parse_quoted_string_state ret =
        parse_single_quoted_string(p, max_length, mp_impl->m_buffer);

    if (!ret.str)
        throw_quoted_string_parse_error("parse_single_quoted_string_value", ret);

    return pstring(ret.str, ret.length);
}

pstring parser_base::parse_double_quoted_string_value(const char*& p, size_t max_length)
{
    parse_quoted_string_state ret =
        parse_double_quoted_string(p, max_length, mp_impl->m_buffer);

    if (!ret.str)
        throw_quoted_string_parse_error("parse_double_quoted_string_value", ret);

    return pstring(ret.str, ret.length);
}

void parser_base::skip_blanks(const char*& p, size_t len)
{
    const char* p_end = p + len;
    for (; p != p_end && *p == ' '; ++p)
        ;
}

void parser_base::start_literal_block()
{
    mp_impl->m_in_literal_block = true;
}

bool parser_base::in_literal_block() const
{
    return mp_impl->m_in_literal_block;
}

void parser_base::handle_line_in_literal(size_t indent)
{
    size_t cur_scope = get_scope();

    if (!has_line_buffer())
    {
        // Start a new multi-line string scope.

        if (indent == cur_scope)
            throw yaml::parse_error("parse: first line of a literal block must be indented.");

        push_scope(indent);
        set_scope_type(yaml::scope_t::multi_line_string);
    }
    else
    {
        // The current scope is already a multi-line scope.
        assert(get_scope_type() == yaml::scope_t::multi_line_string);
        size_t leading_indent = indent - cur_scope;
        prev(leading_indent);
    }

    pstring line = parse_to_end_of_line();
    push_line_back(line.get(), line.size());
}

void parser_base::handle_line_in_multi_line_string()
{
    if (get_scope_type() != yaml::scope_t::multi_line_string)
        set_scope_type(yaml::scope_t::multi_line_string);

    pstring line = parse_to_end_of_line();
    line = line.trim();
    assert(!line.empty());
    push_line_back(line.get(), line.size());
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
