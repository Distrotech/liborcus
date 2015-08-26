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

class pstring;
class yaml_document_tree;

class ORCUS_DLLPUBLIC yaml_document_error : public general_error
{
public:
    yaml_document_error(const std::string& msg);
    virtual ~yaml_document_error() throw();
};

namespace yaml { namespace detail {

class node;
struct yaml_value;

enum class node_t
{
    unset,
    string,
    number,
    map,
    sequence,
    boolean_true,
    boolean_false,
    null
};

class ORCUS_DLLPUBLIC tree_walker
{
    friend class ::orcus::yaml_document_tree;
    friend class map_keys;

    struct impl;
    std::unique_ptr<impl> mp_impl;

    tree_walker(const yaml_document_tree& parent);

public:

    tree_walker() = delete;
    tree_walker(const tree_walker&) = delete;

    tree_walker(tree_walker&& rhs);

    ~tree_walker();

    size_t child_count() const;

    node first_child() const;
};

class ORCUS_DLLPUBLIC node
{
    friend class tree_walker;

    struct impl;
    std::unique_ptr<impl> mp_impl;

    node(const yaml_value* yv);

public:
    node() = delete;

    node(const node& other);
    node(node&& rhs);
    ~node();

    node_t type() const;

    size_t child_count() const;

    node key(size_t index) const;

    node child(size_t index) const;

    pstring string_value() const;

    node& operator=(const node& other);
};

}}

using yaml_node_t = yaml::detail::node_t;

class ORCUS_DLLPUBLIC yaml_document_tree
{
    friend class yaml::detail::tree_walker;

    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    using walker = yaml::detail::tree_walker;
    using node = yaml::detail::node;

    yaml_document_tree();
    ~yaml_document_tree();

    void load(const std::string& strm);

    walker get_walker() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
