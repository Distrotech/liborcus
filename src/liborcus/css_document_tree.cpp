/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/string_pool.hpp"

#include <iostream>
#include <boost/unordered_map.hpp>

using namespace std;

namespace orcus {

namespace {

struct simple_selector_node;

typedef boost::unordered_map<
    css_simple_selector_t, simple_selector_node, css_simple_selector_t::hash> simple_selectors_type;

typedef boost::unordered_map<css_combinator_t, simple_selectors_type> combinators_type;

struct simple_selector_node
{
    css_properties_t properties;
    combinators_type children;
};

css_simple_selector_t intern(string_pool& sp, const css_simple_selector_t& sel)
{
    css_simple_selector_t interned;

    if (!sel.name.empty())
        interned.name = sp.intern(sel.name).first;
    if (!sel.identifier.empty())
        interned.identifier = sp.intern(sel.identifier).first;

    css_simple_selector_t::classes_type::const_iterator it = sel.classes.begin(), ite = sel.classes.end();
    for (; it != ite; ++it)
        interned.classes.insert(sp.intern(*it).first);

    return interned;
}

css_selector_t intern(string_pool& sp, const css_selector_t& sel)
{
    css_selector_t interned;
    interned.first = intern(sp, sel.first);

    css_selector_t::chained_type::const_iterator it = sel.chained.begin(), ite = sel.chained.end();
    for (; it != ite; ++it)
    {
        const css_chained_simple_selector_t& cs = *it;
        css_chained_simple_selector_t cs_interned;
        cs_interned.combinator = cs.combinator;
        cs_interned.simple_selector = intern(sp, cs.simple_selector);
        interned.chained.push_back(cs_interned);
    }

    return interned;
}

void store_properties(
    string_pool& sp, css_properties_t& store, const css_properties_t& props)
{
    css_properties_t::const_iterator it = props.begin(), ite = props.end();
    for (; it != ite; ++it)
    {
        pstring key = sp.intern(it->first).first;
        pstring val = sp.intern(it->second).first;
        store[key] = val;
    }
}

simple_selector_node* get_simple_selector_node(
    simple_selectors_type& store, const css_simple_selector_t& ss)
{
    simple_selectors_type::iterator it = store.find(ss);
    if (it == store.end())
    {
        // Insert this root selector.
        std::pair<simple_selectors_type::iterator, bool> r =
            store.insert(
                simple_selectors_type::value_type(
                    ss, simple_selector_node()));

        if (!r.second)
            // Insertion failed.
            return NULL;

        it = r.first;
    }

    return &it->second;
}

const simple_selector_node* get_simple_selector_node(
    const simple_selectors_type& store, const css_simple_selector_t& ss)
{
    simple_selectors_type::const_iterator it = store.find(ss);
    return it == store.end() ? NULL : &it->second;
}

simple_selectors_type* get_simple_selectors_type(
    combinators_type& store, css_combinator_t combinator)
{
    combinators_type::iterator it = store.find(combinator);
    if (it == store.end())
    {
        // Insert new combinator.
        std::pair<combinators_type::iterator, bool> r =
            store.insert(
                combinators_type::value_type(
                    combinator, simple_selectors_type()));
        if (!r.second)
            // Insertion failed.
            return NULL;

        it = r.first;
    }

    return &it->second;
}

const simple_selectors_type* get_simple_selectors_type(
    const combinators_type& store, css_combinator_t combinator)
{
    combinators_type::const_iterator it = store.find(combinator);
    return it == store.end() ? NULL : &it->second;
}

void dump_properties(const css_properties_t& props)
{
    cout << '{' << endl;
    css_properties_t::const_iterator it = props.begin(), ite = props.end();
    for (; it != ite; ++it)
        cout << "    * " << it->first << ": " << it->second << ';' << endl;
    cout << '}' << endl;
}

}

struct css_document_tree::impl
{
    string_pool m_string_pool;
    simple_selectors_type m_root;
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

    css_selector_t selector_interned = intern(mp_impl->m_string_pool, selector);

    // See if the root selector already exists.
    simple_selector_node* node =
        get_simple_selector_node(mp_impl->m_root, selector_interned.first);

    if (!node)
        // TODO : throw an exception.
        return;

    if (!selector_interned.chained.empty())
    {
        // Follow the chain to find the right node to store new properties.
        css_selector_t::chained_type::iterator it_chain = selector_interned.chained.begin();
        css_selector_t::chained_type::iterator ite_chain = selector_interned.chained.end();
        combinators_type* combos = &node->children;
        for (; it_chain != ite_chain; ++it_chain)
        {
            css_chained_simple_selector_t& css = *it_chain;
            simple_selectors_type* ss = get_simple_selectors_type(*combos, css.combinator);
            if (!ss)
                // TODO : throw an exception.
                return;

            node = get_simple_selector_node(*ss, css.simple_selector);
            if (!node)
                // TODO : throw an exception.
                return;
        }
    }

    // We found the right node to store the properties.
    assert(node);
    store_properties(mp_impl->m_string_pool, node->properties, props);
}

const css_properties_t* css_document_tree::get_properties(const css_selector_t& selector) const
{
    const simple_selector_node* node = get_simple_selector_node(mp_impl->m_root, selector.first);
    if (!node)
        return NULL;

    if (!selector.chained.empty())
    {
        // Follow the chain to find the right node to store new properties.
        css_selector_t::chained_type::const_iterator it_chain = selector.chained.begin();
        css_selector_t::chained_type::const_iterator ite_chain = selector.chained.end();
        const combinators_type* combos = &node->children;
        for (; it_chain != ite_chain; ++it_chain)
        {
            const css_chained_simple_selector_t& css = *it_chain;
            const simple_selectors_type* ss = get_simple_selectors_type(*combos, css.combinator);
            if (!ss)
                return NULL;

            node = get_simple_selector_node(*ss, css.simple_selector);
            if (!node)
                return NULL;
        }
    }

    assert(node);
    return &node->properties;
}

void css_document_tree::dump() const
{
    css_selector_t selector;

    const simple_selectors_type* ss = &mp_impl->m_root;
    simple_selectors_type::const_iterator it_ss = ss->begin(), ite_ss = ss->end();
    for (; it_ss != ite_ss; ++it_ss)
    {
        selector.first = it_ss->first;

        const simple_selector_node* node = &it_ss->second;
        if (!node->properties.empty())
        {
            cout << selector << endl;
            dump_properties(node->properties);
        }

        // TODO : dump chained selectors.
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
