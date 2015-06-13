/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_STRING_POOL_HPP
#define INCLUDED_ORCUS_STRING_POOL_HPP

#include "env.hpp"
#include "pstring.hpp"

#include <string>
#include <memory>

namespace orcus {

/**
 * Implements string hash map.
 */
class ORCUS_PSR_DLLPUBLIC string_pool
{
    string_pool(const string_pool&) = delete;
    string_pool& operator=(const string_pool&) = delete;

public:
    string_pool();
    ~string_pool();

    /**
     * Intern a string.
     *
     * @param str string to intern.  It must be null-terminated.
     *
     * @return pair whose first value is the interned string, and the second
     *         value specifies whether it is a newly created instance (true)
     *         or a reuse of an existing instance (false).
     */
    std::pair<pstring, bool> intern(const char* str);

    /**
     * Intern a string.
     *
     *
     * @param str string to intern.  It doesn't need to be null-terminated.
     * @param n length of the string.
     *
     * @return see above.
     */
    std::pair<pstring, bool> intern(const char* str, size_t n);

    /**
     * Intern a string.
     *
     * @param str string to intern.
     *
     * @return see above.
     */
    std::pair<pstring, bool> intern(const pstring& str);

    void dump() const;
    void clear();
    size_t size() const;

private:
    struct impl;
    std::unique_ptr<impl> mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
