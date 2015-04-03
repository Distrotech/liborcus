/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_document_tree.hpp"
#include "orcus/css_parser.hpp"
#include "orcus/string_pool.hpp"

#define ORCUS_DEBUG_CSS_DOCTREE 0

#include <iostream>
#include <boost/unordered_map.hpp>

using namespace std;

namespace orcus {

namespace {

struct selector_type
{
    css_selector_t selector;
    css::pseudo_element_t pseudo_element;
};

class parser_handler
{
    css_document_tree& m_doc;
    std::vector<selector_type> m_cur_selector_group;
    css_properties_t m_cur_properties;
    pstring m_cur_prop_name;
    std::vector<css_property_value_t> m_cur_prop_values;
    css_selector_t m_cur_selector;  /// current selector
    css_simple_selector_t m_cur_simple_selector;
    css::pseudo_element_t m_cur_pseudo_element;
    css::combinator_t m_cur_combinator;
    bool m_in_prop:1;
public:
    parser_handler(css_document_tree& doc) :
        m_doc(doc),
        m_cur_pseudo_element(0),
        m_cur_combinator(css::combinator_descendant),
        m_in_prop(false) {}

    void at_rule_name(const char* p, size_t n)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "@" << string(p, n).c_str();
#endif
    }

    void simple_selector_type(const char* p, size_t n)
    {
        m_cur_simple_selector.name = pstring(p, n);
    }

    void simple_selector_class(const char* p, size_t n)
    {
        m_cur_simple_selector.classes.insert(pstring(p, n));
    }

    void simple_selector_pseudo_element(css::pseudo_element_t pe)
    {
        // Only the one applied to the last simple selector is valid.
        m_cur_pseudo_element |= pe;
    }

    void simple_selector_pseudo_class(css::pseudo_class_t pc)
    {
        m_cur_simple_selector.pseudo_classes |= pc;
    }

    void simple_selector_id(const char* p, size_t n)
    {
        m_cur_simple_selector.id = pstring(p, n);
    }

    void end_simple_selector()
    {
        if (m_cur_selector.first.empty())
            m_cur_selector.first = m_cur_simple_selector;
        else
        {
            css_chained_simple_selector_t css;
            css.combinator = m_cur_combinator;
            css.simple_selector = m_cur_simple_selector;
            m_cur_selector.chained.push_back(css);
        }

        m_cur_simple_selector.clear();
    }

    void end_selector()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << m_cur_selector << "|";
#endif
        selector_type sel;
        sel.selector = m_cur_selector;
        sel.pseudo_element = m_cur_pseudo_element;
        m_cur_selector_group.push_back(sel);
        m_cur_selector.clear();
        m_cur_pseudo_element = 0;
    }

    void combinator(css::combinator_t combinator)
    {
        m_cur_combinator = combinator;
    }

    void property_name(const char* p, size_t n)
    {
        m_cur_prop_name = pstring(p, n);
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << string(p, n).c_str() << ":";
#endif
    }

    void value(const char* p, size_t n)
    {
        pstring pv(p, n);
        m_cur_prop_values.push_back(pv);
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << " '" << string(p, n).c_str() << "'";
#endif
    }

    void rgb(uint8_t red, uint8_t green, uint8_t blue)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << " rgb(" << (int)red << ',' << (int)green << ',' << (int)blue << ')';
#endif
        css_property_value_t val;
        val.type = css::property_value_rgb;
        val.red = red;
        val.green = green;
        val.blue = blue;
        m_cur_prop_values.push_back(val);
    }

    void rgba(uint8_t red, uint8_t green, uint8_t blue, double alpha)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << " rgba(" << (int)red << ',' << (int)green << ',' << (int)blue << ',' << alpha << ')';
#endif
        css_property_value_t val;
        val.type = css::property_value_rgba;
        val.red = red;
        val.green = green;
        val.blue = blue;
        val.alpha = alpha;
        m_cur_prop_values.push_back(val);
    }

    void hsl(uint8_t hue, uint8_t sat, uint8_t light)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "hsl(" << (int)hue << ',' << (int)sat << ',' << (int)light << ')';
#endif
        css_property_value_t val;
        val.type = css::property_value_hsl;
        val.hue = hue;
        val.saturation = sat;
        val.lightness = light;
        m_cur_prop_values.push_back(val);
    }

    void hsla(uint8_t hue, uint8_t sat, uint8_t light, double alpha)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "hsla(" << (int)hue << ',' << (int)sat << ',' << (int)light << ',' << alpha << ')';
#endif
        css_property_value_t val;
        val.type = css::property_value_hsla;
        val.hue = hue;
        val.saturation = sat;
        val.lightness = light;
        val.alpha = alpha;
        m_cur_prop_values.push_back(val);
    }

    void url(const char* p, size_t n)
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << " url(" << pstring(p, n) << ")";
#endif
        css_property_value_t val;
        val.type = orcus::css::property_value_url;
        val.str = p;
        val.length = n;
        m_cur_prop_values.push_back(val);
    }

    void begin_parse()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "========" << endl;
#endif
    }

    void end_parse()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "========" << endl;
#endif
    }

    void begin_block()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << endl << "{" << endl;
#endif
        m_in_prop = true;
    }

    void end_block()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << "}" << endl;
#endif
        m_in_prop = false;

        // Push the property set and selector group to the document tree.

        std::vector<selector_type>::iterator it = m_cur_selector_group.begin(), ite = m_cur_selector_group.end();
        for (; it != ite; ++it)
            m_doc.insert_properties(it->selector, it->pseudo_element, m_cur_properties);

        m_cur_selector_group.clear();
        m_cur_properties.clear();
    }

    void begin_property()
    {
#if ORCUS_DEBUG_CSS_DOCTREE
        if (m_in_prop)
            cout << "    ";
        cout << "* ";
#endif
    }

    void end_property()
    {
        m_cur_properties.insert(
            css_properties_t::value_type(m_cur_prop_name, m_cur_prop_values));
        m_cur_prop_name.clear();
        m_cur_prop_values.clear();
#if ORCUS_DEBUG_CSS_DOCTREE
        cout << endl;
#endif
    }
};

struct simple_selector_node;

typedef boost::unordered_map<
    css_simple_selector_t, simple_selector_node, css_simple_selector_t::hash> simple_selectors_type;

typedef boost::unordered_map<css::combinator_t, simple_selectors_type> combinators_type;

struct simple_selector_node
{
    css_pseudo_element_properties_t properties;
    combinators_type children;
};

css_simple_selector_t intern(string_pool& sp, const css_simple_selector_t& sel)
{
    css_simple_selector_t interned;

    if (!sel.name.empty())
        interned.name = sp.intern(sel.name).first;
    if (!sel.id.empty())
        interned.id = sp.intern(sel.id).first;

    css_simple_selector_t::classes_type::const_iterator it = sel.classes.begin(), ite = sel.classes.end();
    for (; it != ite; ++it)
        interned.classes.insert(sp.intern(*it).first);

    interned.pseudo_classes = sel.pseudo_classes;

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

class intern_inserter : std::unary_function<css_property_value_t, void>
{
    string_pool& m_sp;
    std::vector<css_property_value_t>& m_dest;
public:
    intern_inserter(string_pool& sp, std::vector<css_property_value_t>& dest) :
        m_sp(sp), m_dest(dest) {}

    void operator() (const css_property_value_t& v) const
    {
        switch (v.type)
        {
            case css::property_value_string:
            case css::property_value_url:
            {
                // String value needs interning.
                css_property_value_t interned = v;
                interned.str = m_sp.intern(v.str, v.length).first.get();
                m_dest.push_back(interned);
            }
            break;
            default:
                m_dest.push_back(v);
        }
    }
};

void store_properties(
    string_pool& sp, css_pseudo_element_properties_t& store,
    css::pseudo_element_t pseudo_flags, const css_properties_t& props)
{
    css_pseudo_element_properties_t::iterator it_store = store.find(pseudo_flags);
    if (it_store == store.end())
    {
        // No storage for this pseudo flag value.  Create a new one.
        std::pair<css_pseudo_element_properties_t::iterator, bool> r =
            store.insert(
                css_pseudo_element_properties_t::value_type(
                    pseudo_flags, css_properties_t()));
        if (!r.second)
            // insertion failed.
            return;

        it_store = r.first;
    }

    css_properties_t& prop_store = it_store->second;

    css_properties_t::const_iterator it = props.begin(), ite = props.end();
    for (; it != ite; ++it)
    {
        pstring key = sp.intern(it->first).first;
        vector<css_property_value_t> vals;
        for_each(it->second.begin(), it->second.end(), intern_inserter(sp, vals));
        prop_store[key] = vals;
    }
}

simple_selector_node* get_or_create_simple_selector_node(
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

simple_selectors_type* get_or_create_simple_selectors_type(
    combinators_type& store, css::combinator_t combinator)
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
    const combinators_type& store, css::combinator_t combinator)
{
    combinators_type::const_iterator it = store.find(combinator);
    return it == store.end() ? NULL : &it->second;
}

void dump_pseudo_elements(css::pseudo_element_t elem)
{
    if (!elem)
        return;

    if (elem & css::pseudo_element_after)
        cout << "::after";
    if (elem & css::pseudo_element_before)
        cout << "::before";
    if (elem & css::pseudo_element_first_letter)
        cout << "::first-letter";
    if (elem & css::pseudo_element_first_line)
        cout << "::first-line";
    if (elem & css::pseudo_element_selection)
        cout << "::selection";
    if (elem & css::pseudo_element_backdrop)
        cout << "::backdrop";
}

void dump_properties(const css_properties_t& props)
{
    cout << '{' << endl;
    css_properties_t::const_iterator it = props.begin(), ite = props.end();
    for (; it != ite; ++it)
    {
        cout << "    * " << it->first << ": ";
        const vector<css_property_value_t>& vals = it->second;
        copy(vals.begin(), vals.end(), ostream_iterator<css_property_value_t>(cout, " "));
        cout << ';' << endl;
    }
    cout << '}' << endl;
}

void dump_all_properties(const css_selector_t& selector, const css_pseudo_element_properties_t& properties)
{
    css_pseudo_element_properties_t::const_iterator it_prop = properties.begin(), ite_prop = properties.end();
    for (; it_prop != ite_prop; ++it_prop)
    {
        const css_properties_t& prop = it_prop->second;
        if (prop.empty())
            continue;

        cout << selector;
        dump_pseudo_elements(it_prop->first);
        cout << endl;
        dump_properties(prop);
    }
}

void dump_chained_recursive(
    css_selector_t& selector, css::combinator_t op, const simple_selectors_type& ss)
{
    simple_selectors_type::const_iterator it_ss = ss.begin(), ite_ss = ss.end();
    for (; it_ss != ite_ss; ++it_ss)
    {
        css_chained_simple_selector_t chained_ss;
        chained_ss.combinator = op;
        chained_ss.simple_selector = it_ss->first;
        selector.chained.push_back(chained_ss);

        const simple_selector_node& node = it_ss->second;
        dump_all_properties(selector, node.properties);

        combinators_type::const_iterator it_comb = node.children.begin(), ite_comb = node.children.end();
        for (; it_comb != ite_comb; ++it_comb)
            dump_chained_recursive(selector, it_comb->first, it_comb->second);

        selector.chained.pop_back();
    }
}

const css_pseudo_element_properties_t* get_properties_map(
    const simple_selectors_type& root, const css_selector_t& selector)
{
    const simple_selector_node* node = get_simple_selector_node(root, selector.first);
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

            combos = &node->children;
        }
    }

    assert(node);
    return &node->properties;
}

}

css_document_tree::insertion_error::insertion_error(const std::string& msg) :
    general_error(msg) {}

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

void css_document_tree::load(const std::string& strm)
{
    if (strm.empty())
        return;

#if ORCUS_DEBUG_CSS_DOCTREE
    cout << "original: '" << strm << "'" << endl << endl;
#endif

    parser_handler handler(*this);
    css_parser<parser_handler> parser(&strm[0], strm.size(), handler);
    parser.parse();
}

void css_document_tree::insert_properties(
    const css_selector_t& selector,
    css::pseudo_element_t pseudo_elem,
    const css_properties_t& props)
{
    if (props.empty())
        return;

    css_selector_t selector_interned = intern(mp_impl->m_string_pool, selector);

    // See if the root selector already exists.
    simple_selector_node* node =
        get_or_create_simple_selector_node(mp_impl->m_root, selector_interned.first);

    if (!node)
        throw insertion_error("failed to find or create the root simple selector node.");

    if (!selector_interned.chained.empty())
    {
        // Follow the chain to find the right node to store new properties.
        css_selector_t::chained_type::iterator it_chain = selector_interned.chained.begin();
        css_selector_t::chained_type::iterator ite_chain = selector_interned.chained.end();
        combinators_type* combos = &node->children;
        for (; it_chain != ite_chain; ++it_chain)
        {
            css_chained_simple_selector_t& css = *it_chain;
            simple_selectors_type* ss = get_or_create_simple_selectors_type(*combos, css.combinator);
            if (!ss)
                throw insertion_error("failed to find or create the simple selectors type by combinator.");

            node = get_or_create_simple_selector_node(*ss, css.simple_selector);
            if (!node)
                throw insertion_error("failed to find or create the simple selector node.");

            combos = &node->children;
        }
    }

    // We found the right node to store the properties.
    assert(node);
    store_properties(mp_impl->m_string_pool, node->properties, pseudo_elem, props);
}

const css_properties_t* css_document_tree::get_properties(
    const css_selector_t& selector, css::pseudo_element_t pseudo_elem) const
{
    const css_pseudo_element_properties_t* prop_map = get_properties_map(mp_impl->m_root, selector);
    if (!prop_map)
        return NULL;

    css_pseudo_element_properties_t::const_iterator it = prop_map->find(pseudo_elem);
    if (it == prop_map->end())
        return NULL;

    return &it->second;
}

const css_pseudo_element_properties_t*
css_document_tree::get_all_properties(const css_selector_t& selector) const
{
    return get_properties_map(mp_impl->m_root, selector);
}

void css_document_tree::dump() const
{
    css_selector_t selector;

    const simple_selectors_type& ss = mp_impl->m_root;
    simple_selectors_type::const_iterator it_ss = ss.begin(), ite_ss = ss.end();
    for (; it_ss != ite_ss; ++it_ss)
    {
        selector.first = it_ss->first;

        const simple_selector_node& node = it_ss->second;
        dump_all_properties(selector, node.properties);

        combinators_type::const_iterator it_comb = node.children.begin(), ite_comb = node.children.end();
        for (; it_comb != ite_comb; ++it_comb)
            dump_chained_recursive(selector, it_comb->first, it_comb->second);
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
