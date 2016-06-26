/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_parser_thread.hpp"
#include "orcus/global.hpp"
#include "orcus/json_parser.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/detail/parser_token_buffer.hpp"

#include <sstream>
#include <algorithm>
#include <limits>

namespace orcus { namespace json {

parse_token::parse_token() : type(parse_token_t::unknown) {}

parse_token::parse_token(parse_token_t _type) : type(_type) {}

parse_token::parse_token(parse_token_t _type, const char* p, size_t len) :
    type(_type)
{
    string_value.p = p;
    string_value.len = len;
}

parse_token::parse_token(parse_token_t _type, const char* p, size_t len, std::ptrdiff_t offset) :
    type(_type)
{
    error_value.p = p;
    error_value.len = len;
    error_value.offset = offset;
}

parse_token::parse_token(double value) :
    type(parse_token_t::number), numeric_value(value) {}

parse_token::parse_token(const parse_token& other) :
    type(other.type)
{
    switch (type)
    {
        case parse_token_t::object_key:
        case parse_token_t::string:
            string_value.p = other.string_value.p;
            string_value.len = other.string_value.len;
            break;
        case parse_token_t::number:
            numeric_value = other.numeric_value;
            break;
        case parse_token_t::parse_error:
            error_value.p = other.error_value.p;
            error_value.len = other.error_value.len;
            error_value.offset = other.error_value.offset;
            break;
        default:
            ;
    }
}

bool parse_token::operator== (const parse_token& other) const
{
    if (type != other.type)
        return false;

    switch (type)
    {
        case parse_token_t::object_key:
        case parse_token_t::string:
            return pstring(string_value.p, string_value.len) == pstring(other.string_value.p, other.string_value.len);
        case parse_token_t::number:
            return numeric_value == other.numeric_value;
        case parse_token_t::parse_error:
            if (pstring(error_value.p, error_value.len) != pstring(other.error_value.p, other.error_value.len))
                return false;
            if (error_value.offset != error_value.offset)
                return false;
            break;
        default:
            ;
    }
    return true;
}

bool parse_token::operator!= (const parse_token& other) const
{
    return !operator== (other);
}

/**
 * This impl class also acts as a handler for the parser.
 *
 */
struct parser_thread::impl
{
    detail::thread::parser_token_buffer<parse_tokens_t> m_token_buffer;
    string_pool m_pool;
    parse_tokens_t m_parser_tokens; // token buffer for the parser thread.

    const char* mp_char;
    size_t m_size;

    impl(const char* p, size_t n, size_t min_token_size, size_t max_token_size) :
        m_token_buffer(min_token_size, max_token_size),
        mp_char(p), m_size(n)
    {
        m_parser_tokens.reserve(min_token_size);
    }

    void start()
    {
        try
        {
            json_parser<parser_thread::impl> parser(mp_char, m_size, *this);
            parser.parse();
        }
        catch (const parse_error& e)
        {
            pstring s = m_pool.intern(e.what()).first;
            m_parser_tokens.emplace_back(parse_token_t::parse_error, s.get(), s.size(), e.offset());
        }

        notify_and_finish();
    }

    void begin_parse()
    {
        m_parser_tokens.emplace_back(parse_token_t::begin_parse);
        check_and_notify();
    }

    void end_parse()
    {
        m_parser_tokens.emplace_back(parse_token_t::end_parse);
        check_and_notify();
    }

    void begin_array()
    {
        m_parser_tokens.emplace_back(parse_token_t::begin_array);
        check_and_notify();
    }

    void end_array()
    {
        m_parser_tokens.emplace_back(parse_token_t::end_array);
        check_and_notify();
    }

    void begin_object()
    {
        m_parser_tokens.emplace_back(parse_token_t::begin_object);
        check_and_notify();
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        if (transient)
        {
            pstring s = m_pool.intern(p, len).first;
            p = s.get();
            len = s.size();
        }

        m_parser_tokens.emplace_back(parse_token_t::object_key, p, len);
        check_and_notify();
    }

    void end_object()
    {
        m_parser_tokens.emplace_back(parse_token_t::end_object);
        check_and_notify();
    }

    void boolean_true()
    {
        m_parser_tokens.emplace_back(parse_token_t::boolean_true);
        check_and_notify();
    }

    void boolean_false()
    {
        m_parser_tokens.emplace_back(parse_token_t::boolean_false);
        check_and_notify();
    }

    void null()
    {
        m_parser_tokens.emplace_back(parse_token_t::null);
        check_and_notify();
    }

    void string(const char* p, size_t len, bool transient)
    {
        if (transient)
        {
            pstring s = m_pool.intern(p, len).first;
            p = s.get();
            len = s.size();
        }

        m_parser_tokens.emplace_back(parse_token_t::string, p, len);
        check_and_notify();
    }

    void number(double val)
    {
        m_parser_tokens.emplace_back(val);
        check_and_notify();
    }

    void check_and_notify()
    {
        m_token_buffer.check_and_notify(m_parser_tokens);
    }

    void notify_and_finish()
    {
        m_token_buffer.notify_and_finish(m_parser_tokens);
    }

    bool next_tokens(parse_tokens_t& tokens)
    {
        return m_token_buffer.next_tokens(tokens);
    }

    parser_stats get_stats() const
    {
        parser_stats stats;
        stats.token_buffer_size_threshold = m_token_buffer.token_size_threshold();
        return stats;
    }

    void swap_string_pool(string_pool& pool)
    {
        m_pool.swap(pool);
    }
};

std::ostream& operator<< (std::ostream& os, const parse_tokens_t& tokens)
{
    using std::endl;

    os << "token size: " << tokens.size() << endl;

    std::for_each(tokens.begin(), tokens.end(),
        [&](const parse_token& t)
        {
            switch (t.type)
            {
                case parse_token_t::begin_array:
                    os << "- begin_array" << endl;
                    break;
                case parse_token_t::begin_object:
                    os << "- begin_object" << endl;
                    break;
                case parse_token_t::begin_parse:
                    os << "- begin_parse" << endl;
                    break;
                case parse_token_t::boolean_false:
                    os << "- boolean_false" << endl;
                    break;
                case parse_token_t::boolean_true:
                    os << "- boolean_true" << endl;
                    break;
                case parse_token_t::end_array:
                    os << "- end_array" << endl;
                    break;
                case parse_token_t::end_object:
                    os << "- end_object" << endl;
                    break;
                case parse_token_t::end_parse:
                    os << "- end_parse" << endl;
                    break;
                case parse_token_t::null:
                    os << "- null" << endl;
                    break;
                case parse_token_t::number:
                    os << "- number (v=" << t.numeric_value << ")" << endl;
                    break;
                case parse_token_t::object_key:
                    os << "- object_key (v=" << pstring(t.string_value.p, t.string_value.len) << ")" << endl;
                    break;
                case parse_token_t::parse_error:
                    os << "- parse_error (v=" << pstring(t.error_value.p, t.error_value.len) << ", offset=" << t.error_value.offset << ")" << endl;
                    break;
                case parse_token_t::string:
                    os << "- string (" << pstring(t.string_value.p, t.string_value.len) << ")" << endl;
                    break;
                case parse_token_t::unknown:
                    os << "- unknown" << endl;
                    break;
                default:
                    ;
            }
        }
    );

    return os;
}

parser_thread::parser_thread(const char* p, size_t n, size_t min_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>(
        p, n, min_token_size, std::numeric_limits<size_t>::max()/2)) {}

parser_thread::parser_thread(const char* p, size_t n, size_t min_token_size, size_t max_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>(
        p, n, min_token_size, max_token_size)) {}

parser_thread::~parser_thread() {}

void parser_thread::start()
{
    mp_impl->start();
}

bool parser_thread::next_tokens(parse_tokens_t& tokens)
{
    return mp_impl->next_tokens(tokens);
}

parser_stats parser_thread::get_stats() const
{
    return mp_impl->get_stats();
}

void parser_thread::swap_string_pool(string_pool& pool)
{
    mp_impl->swap_string_pool(pool);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
