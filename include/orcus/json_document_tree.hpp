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

    void load(const std::string& strm);

    std::string dump() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
