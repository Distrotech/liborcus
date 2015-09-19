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
#include <vector>

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

class ORCUS_DLLPUBLIC node
{
    friend class ::orcus::yaml_document_tree;

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

    std::vector<node> keys() const;

    node key(size_t index) const;

    node child(size_t index) const;

    node child(const node& key) const;

    node parent() const;

    pstring string_value() const;
    double numeric_value() const;

    node& operator=(const node& other);

    uintptr_t identity() const;
};

}}

using yaml_node_t = yaml::detail::node_t;

class ORCUS_DLLPUBLIC yaml_document_tree
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    using node = yaml::detail::node;

    yaml_document_tree();
    ~yaml_document_tree();

    void load(const std::string& strm);

    size_t get_document_count() const;

    node get_document_root(size_t index) const;

    std::string dump_yaml() const;

    std::string dump_json() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
