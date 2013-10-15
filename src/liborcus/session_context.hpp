/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SESSION_CONTEXT_HPP
#define ORCUS_SESSION_CONTEXT_HPP

#include "orcus/string_pool.hpp"

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace orcus {

struct session_context : boost::noncopyable
{
    string_pool m_string_pool;

    /**
     * Derive from this class in case the filter needs to store its own
     * session data.
     */
    struct custom_data
    {
        virtual ~custom_data() = 0;
    };

    boost::scoped_ptr<custom_data> mp_data;

    ~session_context();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
