/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CSS_DOCUMENT_TREE_HPP
#define INCLUDED_ORCUS_CSS_DOCUMENT_TREE_HPP

#include "orcus/css_selector.hpp"

#include <string>

namespace orcus {

/**
 * Class representing CSS rules.
 */
class ORCUS_DLLPUBLIC css_document_tree
{
    struct impl;
    impl* mp_impl;

public:

    css_document_tree();
    ~css_document_tree();

    /**
     * Load raw string stream containing CSS rules to populate the document
     * tree.
     *
     * @param strm stream containing raw CSS rules.
     */
    void load(const std::string& strm);

    /**
     * Insert or replace properties for given selector and pseudo element
     * flags.
     *
     * @param selector selector to store properties for.
     * @param pseudo_elem pseudo element flags for the last simple selector.
     * @param props new properties to insert.
     */
    void insert_properties(
        const css_selector_t& selector,
        css::pseudo_element_t pseudo_elem,
        const css_properties_t& props);

    /**
     * Get properties associated with given selector.
     *
     * @param selector selector to get properties for.
     * @param pseudo_elem pseudo element flags for the last simple selector.
     *
     * @return const pointer to the property set instance, or NULL in case
     *         there is no properties for the given selector.
     */
    const css_properties_t* get_properties(
        const css_selector_t& selector, css::pseudo_element_t pseudo_elem) const;

    void dump() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
