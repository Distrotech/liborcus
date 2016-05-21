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

#include <mutex>
#include <condition_variable>
#include <sstream>
#include <algorithm>

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
    string_pool m_pool;

    std::mutex m_mtx;
    std::condition_variable m_cv;
    parse_tokens_t m_client_tokens; // token buffer used to hand over tokens to the client.
    bool m_in_progress;

    parse_tokens_t m_parser_tokens; // token buffer for the parser.

    const char* mp_char;
    size_t m_size;
    size_t m_max_token_size;

    impl(const char* p, size_t n, size_t max_token_size) :
        m_in_progress(true), mp_char(p), m_size(n), m_max_token_size(max_token_size) {}

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
        if (m_parser_tokens.size() <= m_max_token_size)
            // Still below the threshold.
            return;

        std::unique_lock<std::mutex> lock(m_mtx);
        m_client_tokens.swap(m_parser_tokens);
        m_cv.notify_one();
    }

    void notify_and_finish()
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_client_tokens.swap(m_parser_tokens);
        m_in_progress = false;
        m_cv.notify_one();
    }

    bool next_tokens(parse_tokens_t& tokens)
    {
        tokens.clear(); // This ensures that m_parser_tokens will eventually become empty.

        std::unique_lock<std::mutex> lock(m_mtx);
        while (m_client_tokens.empty())
            m_cv.wait(lock);

        tokens.swap(m_client_tokens);
        return m_in_progress;
    }
};

parser_thread::parser_thread(const char* p, size_t n, size_t max_token_size) :
    mp_impl(orcus::make_unique<parser_thread::impl>(p, n, max_token_size)) {}

parser_thread::~parser_thread() {}

void parser_thread::start()
{
    mp_impl->start();
}

bool parser_thread::next_tokens(parse_tokens_t& tokens)
{
    return mp_impl->next_tokens(tokens);
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
