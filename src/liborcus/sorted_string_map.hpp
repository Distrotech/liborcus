/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_SORTED_STRING_MAP_HPP
#define ORCUS_SORTED_STRING_MAP_HPP

#include <cstdlib>

namespace orcus {

template<typename _ValueT>
class sorted_string_map
{
public:
    typedef _ValueT value_type;
    typedef size_t size_type;

    struct entry
    {
        const char* key;
        value_type value;
    };

    sorted_string_map(const entry* entries, size_type entry_size, value_type null_value) :
        m_entries(entries),
        m_null_value(null_value),
        m_entry_size(entry_size),
        m_entry_end(m_entries+m_entry_size) {}

    value_type find(const char* input) const
    {
        const entry* p = m_entries;
        size_t pos = 0;
        for (; p != m_entry_end; ++p)
        {
            const char* key = p->key;
            for (; input[pos] && key[pos]; ++pos)
            {
                if (input[pos] != key[pos])
                    // Move to the next entry.
                    break;
            }

            if (input[pos] == 0 && key[pos] == 0)
            {
                // Match found!
                return p->value;
            }
        }
        return m_null_value;
    }

    size_type size() const { return m_entry_size; }

private:

    const entry* m_entries;
    value_type m_null_value;
    size_type m_entry_size;
    const entry* m_entry_end;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
