/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/cell_buffer.hpp"

#include <cstring>

#define ORCUS_DEBUG_CELL_BUFFER 0

#if ORCUS_DEBUG_CELL_BUFFER
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace orcus {

cell_buffer::cell_buffer() : m_buf_size(0) {}

void cell_buffer::append(const char* p, size_t len)
{
    if (!len)
        return;

#if ORCUS_DEBUG_CELL_BUFFER
    cout << "cell_buffer::append: '" << std::string(p, len) << "'" << endl;
#endif

    size_t size_needed = m_buf_size + len;
    if (m_buffer.size() < size_needed)
        m_buffer.resize(size_needed);

    char* p_dest = &m_buffer[m_buf_size];
    std::strncpy(p_dest, p, len);
    m_buf_size += len;
}

void cell_buffer::reset()
{
    m_buf_size = 0;
}

const char* cell_buffer::get() const
{
    return &m_buffer[0];
}

size_t cell_buffer::size() const
{
    return m_buf_size;
}

bool cell_buffer::empty() const
{
    return m_buf_size == 0;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
