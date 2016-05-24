/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_DETAIL_THREAD_HPP
#define INCLUDED_ORCUS_DETAIL_THREAD_HPP

#include <thread>

namespace orcus { namespace detail { namespace thread {

class scoped_guard
{
    std::thread m_thread;
public:
    scoped_guard(std::thread thread) : m_thread(std::move(thread)) {}
    scoped_guard(scoped_guard&& other) : m_thread(std::move(other.m_thread)) {}

    scoped_guard(const scoped_guard&) = delete;
    scoped_guard& operator= (const scoped_guard&) = delete;

    ~scoped_guard()
    {
        m_thread.join();
    }
};

}}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
