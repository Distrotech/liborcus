/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/string_pool.hpp"

#include <iostream>

using namespace std;

namespace orcus {

struct css_document_tree::impl
{
    string_pool m_string_pool;
};

css_document_tree::css_document_tree() : mp_impl(new impl)
{
}

css_document_tree::~css_document_tree()
{
    delete mp_impl;
}

void css_document_tree::insert_properties(
    const css_selector_t& selector, const css_properties_t& props)
{
    if (props.empty())
        return;

    cout << "--" << endl;
    cout << selector << endl;
    cout << '{' << endl;

    css_properties_t::const_iterator it = props.begin(), ite = props.end();
    for (; it != ite; ++it)
    {
        cout << "    * " << it->first << ": " << it->second << ';' << endl;
    }

    cout << '}' << endl;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
