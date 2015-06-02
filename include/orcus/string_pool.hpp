/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_STRING_POOL_HPP__
#define __ORCUS_STRING_POOL_HPP__

#include <string>
#include <boost/unordered_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/functional/hash.hpp>

#include "env.hpp"
#include "pstring.hpp"

namespace orcus {

/**
 * Implements string hash map.
 */
class ORCUS_PSR_DLLPUBLIC string_pool
{
    struct string_hash
    {
        size_t operator() (const std::string* p) const;
    private:
        boost::hash<std::string> m_hash;
    };

    struct string_equal_to
    {
        bool operator() (const std::string* p1, const std::string* p2) const;
    private:
        std::equal_to<std::string> m_equal_to;
    };

    typedef boost::unordered_set<pstring, pstring::hash> string_set_type;
    typedef boost::ptr_vector<std::string> string_store_type;

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
    string_set_type m_set;
    string_store_type m_store;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
