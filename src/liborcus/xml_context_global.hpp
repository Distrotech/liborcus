/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_XML_CONTEXT_GLOBAL_HPP
#define ORCUS_XML_CONTEXT_GLOBAL_HPP

#include "orcus/types.hpp"
#include "orcus/pstring.hpp"

#include <functional>

namespace orcus {

class string_pool;

/**
 * Use this just to get the value of a single attribute for a given element.
 */
class single_attr_getter : public std::unary_function<xml_token_attr_t, void>
{
    string_pool* m_pool;
    pstring m_value;
    xmlns_id_t m_ns;
    xml_token_t m_name;

public:
    single_attr_getter(string_pool& pool, xmlns_id_t ns, xml_token_t name);
    void operator() (const xml_token_attr_t& attr);
    pstring get_value() const;

    static pstring get(const std::vector<xml_token_attr_t>& attrs, string_pool& pool, xmlns_id_t ns, xml_token_t name);
};

class single_long_attr_getter : public std::unary_function<xml_token_attr_t, void>
{
    long m_value;
    xmlns_id_t m_ns;
    xml_token_t m_name;

public:
    single_long_attr_getter(xmlns_id_t ns, xml_token_t name);
    void operator() (const xml_token_attr_t& attr);
    long get_value() const;

    static long get(const std::vector<xml_token_attr_t>& attrs, xmlns_id_t ns, xml_token_t name);
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
