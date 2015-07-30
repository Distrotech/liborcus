/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_DOCUMENT_TREE_HPP
#define INCLUDED_ORCUS_JSON_DOCUMENT_TREE_HPP

#include "orcus/env.hpp"

#include <string>
#include <memory>

namespace orcus {

class ORCUS_DLLPUBLIC json_document_tree
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    json_document_tree();
    ~json_document_tree();

    /**
     * Load raw string stream containing a JSON structure to populate the
     * document tree.
     *
     * @param strm stream containing a JSON structure.
     */
    void load(const std::string& strm);

    /**
     * Dump the JSON document tree to string.
     *
     * @return a string representation of the JSON document tree.
     */
    std::string dump() const;

    /**
     * Dump the JSON document tree to an XML structure.
     *
     *
     * @return a string containing an XML structure representing the JSON
     *         content.
     */
    std::string dump_xml() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
