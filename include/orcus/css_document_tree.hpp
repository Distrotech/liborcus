/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_CSS_DOCUMENT_TREE_HPP
#define INCLUDED_ORCUS_CSS_DOCUMENT_TREE_HPP

#include "orcus/css_types.hpp"

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

    void insert_properties(const css_selector_t& selector, const css_properties_t& props);

    void dump() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
