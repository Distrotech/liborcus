/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_context_global.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/measurement.hpp"

namespace orcus {

single_attr_getter::single_attr_getter(string_pool& pool, xmlns_id_t ns, xml_token_t name) :
    m_pool(pool), m_ns(ns), m_name(name) {}

void single_attr_getter::operator() (const xml_token_attr_t& attr)
{
    if (attr.ns != m_ns || attr.name != m_name)
        return;

    m_value = attr.value;
    if (attr.transient)
        m_value = m_pool.intern(m_value).first;
}

pstring single_attr_getter::get_value() const
{
    return m_value;
}

single_long_attr_getter::single_long_attr_getter(xmlns_id_t ns, xml_token_t name) :
    m_ns(ns), m_name(name) {}

void single_long_attr_getter::operator() (const xml_token_attr_t& attr)
{
    if (attr.ns != m_ns || attr.name != m_name)
        return;

    m_value = to_long(attr.value);
}

long single_long_attr_getter::get_value() const
{
    return m_value;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
