/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_DETAIL_THREAD_PARSER_TOKEN_BUFFER_HPP
#define INCLUDED_ORCUS_DETAIL_THREAD_PARSER_TOKEN_BUFFER_HPP

#include "orcus/exception.hpp"

#include <mutex>
#include <condition_variable>

namespace orcus { namespace detail { namespace thread {

/**
 * Class that manages synchronization of parser tokens used in
 * multi-threaded parsers.
 */
template<typename _TokensT>
class parser_token_buffer
{
    typedef _TokensT tokens_type;

    mutable std::mutex m_mtx_tokens;
    std::condition_variable m_cv_tokens_empty;
    std::condition_variable m_cv_tokens_ready;

    tokens_type m_tokens; // token buffer used to hand over tokens to the client.

    size_t m_token_size_threshold;
    const size_t m_max_token_size;

    bool m_parsing_progress;

    bool tokens_empty() const
    {
        std::unique_lock<std::mutex> lock(m_mtx_tokens);
        return m_tokens.empty();
    }

    /**
     * Only to be called from the parser thread.
     *
     * Wait until the processor thread takes the new tokens and makes the
     * token buffer empty.
     */
    void wait_until_tokens_empty()
    {
        std::unique_lock<std::mutex> lock(m_mtx_tokens);
        while (!m_tokens.empty())
            m_cv_tokens_empty.wait(lock);
    }

public:
    parser_token_buffer(size_t min_token_size, size_t max_token_size) :
        m_token_size_threshold(std::max<size_t>(min_token_size, 1)),
        m_max_token_size(max_token_size),
        m_parsing_progress(true)
    {
        if (m_token_size_threshold > m_max_token_size)
            throw invalid_arg_error(
                "initial token size threshold is already larger than the max token size.");
    }

    /**
     * Check the size of the parser token buffer, and if it exceeds specified
     * threshold, move it to the client buffer.
     *
     * Call this from the parser thread.
     *
     * @param parser_tokens parser token buffer.
     */
    void check_and_notify(tokens_type& parser_tokens)
    {
        if (parser_tokens.size() < m_token_size_threshold)
            // Still below the threshold.
            return;

        if (!tokens_empty())
        {
            if (m_token_size_threshold < (m_max_token_size/2))
            {
                // Double the threshold and continue to parse.
                m_token_size_threshold *= 2;
                return;
            }

            // We cannot increase the threshold any more.  Wait for the
            // client to finish.
            wait_until_tokens_empty();
        }

        std::unique_lock<std::mutex> lock(m_mtx_tokens);
        m_tokens.swap(parser_tokens);
        lock.unlock();
        m_cv_tokens_ready.notify_one();
    }

    /**
     * Move the current parser token buffer to the client buffer, and signal
     * the end of parsing.
     *
     * Call this from the parser thread.
     *
     * @param parser_tokens parser token buffer.
     */
    void notify_and_finish(tokens_type& parser_tokens)
    {
        // Wait until the client tokens get used up.
        wait_until_tokens_empty();

        std::unique_lock<std::mutex> lock(m_mtx_tokens);
        m_tokens.swap(parser_tokens);
        m_parsing_progress = false;
        lock.unlock();
        m_cv_tokens_ready.notify_one();
    }

    /**
     * Retrieve the tokens currently in the client token buffer.
     *
     * Call this from the client (non-parser) thread.
     *
     * @param tokens place to move the tokens in the client token buffer to.
     *
     * @return true if the parsing is still in progress, therefore more tokens
     *         are expected, false if this is the last set of tokens.
     */
    bool next_tokens(tokens_type& tokens)
    {
        tokens.clear();

        // Wait until the parser passes a new set of tokens.
        std::unique_lock<std::mutex> lock(m_mtx_tokens);
        while (m_tokens.empty() && m_parsing_progress)
            m_cv_tokens_ready.wait(lock);

        // Get the new tokens and notify the parser.
        tokens.swap(m_tokens);
        bool parsing_progress = m_parsing_progress; // Make a copy so that lock can be released safely.

        lock.unlock();

        m_cv_tokens_empty.notify_one();

        return parsing_progress;
    }

    /**
     * Return the current token size threshold.  Call this only after the
     * parsing has finished.
     *
     * @return current token size threshold.
     */
    size_t token_size_threshold() const
    {
        if (m_parsing_progress)
            return 0;

        return m_token_size_threshold;
    }
};

}}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
