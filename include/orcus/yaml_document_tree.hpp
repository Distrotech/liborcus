/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_YAML_DOCUMENT_TREE_HPP
#define INCLUDED_ORCUS_YAML_DOCUMENT_TREE_HPP

#include "orcus/env.hpp"
#include "orcus/exception.hpp"

#include <string>
#include <memory>

namespace orcus {

class yaml_document_tree;

class ORCUS_DLLPUBLIC yaml_document_error : public general_error
{
public:
    yaml_document_error(const std::string& msg);
    virtual ~yaml_document_error() throw();
};

namespace yaml { namespace detail {

class ORCUS_DLLPUBLIC tree_walker
{
    friend class ::orcus::yaml_document_tree;

    struct impl;
    std::unique_ptr<impl> mp_impl;

    tree_walker(const yaml_document_tree& parent);

public:

    enum class node_type
    {
        unset,
        document_list,
        string,
        number,
        map,
        sequence,
        boolean_true,
        boolean_false,
        null
    };

    tree_walker() = delete;
    tree_walker(const tree_walker&) = delete;

    tree_walker(tree_walker&& rhs);

    ~tree_walker();

    /**
     * Returns the type of current node.
     *
     * @return current node type.
     */
    node_type type() const;

    size_t child_count() const;

    void first_child();
};

}}

class ORCUS_DLLPUBLIC yaml_document_tree
{
    friend class yaml::detail::tree_walker;

    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    typedef yaml::detail::tree_walker walker;

    yaml_document_tree();
    ~yaml_document_tree();

    void load(const std::string& strm);

    walker get_walker() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
