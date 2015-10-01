/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_DOCUMENT_TREE_HPP
#define INCLUDED_ORCUS_JSON_DOCUMENT_TREE_HPP

#include "orcus/env.hpp"
#include "orcus/exception.hpp"

#include <string>
#include <memory>
#include <vector>

namespace orcus {

class ORCUS_DLLPUBLIC json_document_error : public general_error
{
public:
    json_document_error(const std::string& msg);
    virtual ~json_document_error() throw();
};

struct json_config;
class json_document_tree;
class pstring;

namespace json { namespace detail {

struct json_value;

enum class node_t
{
    unset,
    string,
    number,
    object,
    array,
    boolean_true,
    boolean_false,
    null
};

class ORCUS_DLLPUBLIC node
{
    friend class ::orcus::json_document_tree;

    struct impl;
    std::unique_ptr<impl> mp_impl;

    node(const json_value* jv);

public:
    node() = delete;

    node(const node& other);
    node(node&& rhs);
    ~node();

    node_t type() const;

    size_t child_count() const;

    std::vector<pstring> keys() const;

    pstring key(size_t index) const;

    node child(size_t index) const;

    node child(const pstring& key) const;

    node parent() const;

    pstring string_value() const;
    double numeric_value() const;

    node& operator=(const node& other);

    /**
     * Return an indentifier of the JSON value object that the node
     * represents.  The identifier is derived directly from the memory address
     * of the value object.
     *
     * @return identifier of the JSON value object.
     */
    uintptr_t identity() const;
};

}}

using json_node_t = json::detail::node_t;

class ORCUS_DLLPUBLIC json_document_tree
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    using node = json::detail::node;

    json_document_tree();
    ~json_document_tree();

    /**
     * Load raw string stream containing a JSON structure to populate the
     * document tree.
     *
     * @param strm stream containing a JSON structure.
     */
    void load(const std::string& strm, const json_config& config);

    void load(const char* p, size_t n, const json_config& config);

    /**
     * Get the root node of the document.
     *
     * @return root node of the document.
     */
    node get_document_root() const;

    /**
     * Dump the JSON document tree to string.
     *
     * @return a string representation of the JSON document tree.
     */
    std::string dump() const;

    /**
     * Dump the JSON document tree to an XML structure.
     *
     * @return a string containing an XML structure representing the JSON
     *         content.
     */
    std::string dump_xml() const;
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
