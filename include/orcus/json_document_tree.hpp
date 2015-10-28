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

class string_pool;

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
    /** node type is not set.  */
    unset,
    /** JSON string node.  A node of this type contains a string value. */
    string,
    /** JSON number node. A node of this type contains a numeric value. */
    number,
    /**
     * JSON object node.  A node of this type contains one or more key-value
     * pairs.
     */
    object,
    /**
     * JSON array node.  A node of this type contains one or more child nodes.
     */
    array,
    /**
     * JSON boolean node containing a value of 'true'.
     */
    boolean_true,
    /**
     * JSON boolean node containing a value of 'false'.
     */
    boolean_false,
    /**
     * JSON node containing a 'null' value.
     */
    null
};

/**
 * Each node instance represents a JSON value object stored in the document
 * tree.
 */
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

    /**
     * Get the type of a node.
     *
     * @return node type.
     */
    node_t type() const;

    /**
     * Get the number of child nodes if any.
     *
     * @return number of child nodes.
     */
    size_t child_count() const;

    /**
     * Get a list of keys stored in a JSON object node.
     *
     * @exception orcus::json_document_error if the node is not of the object
     *                 type.
     * @return a list of keys.
     */
    std::vector<pstring> keys() const;

    /**
     * Get the key by index in a JSON object node.  This method works only
     * when the <b>preserve object order</b> option is set.
     *
     * @param index 0-based key index.
     *
     * @exception orcus::json_document_error if the node is not of the object
     *                 type.
     *
     * @exception std::out_of_range if the index is equal to or greater than
     *               the number of keys stored in the node.
     *
     * @return key value.
     */
    pstring key(size_t index) const;

    /**
     * Get a child node by index.
     *
     * @param index 0-based index of a child node.
     *
     * @exception orcus::json_document_error if the node is not one of the
     *                 object or array types.
     *
     * @exception std::out_of_range if the index is equal to or greater than
     *               the number of child nodes that the node has.
     *
     * @return child node instance.
     */
    node child(size_t index) const;

    /**
     * Get a child node by textural key value.
     *
     * @param key textural key value to get a child node by.
     *
     * @exception orcus::json_document_error if the node is not of the object
     *                 type, or the node doesn't have the specified key.
     *
     * @return child node instance.
     */
    node child(const pstring& key) const;

    /**
     * Get the parent node.
     *
     * @exception orcus::json_document_error if the node doesn't have a parent
     *                 node which implies that the node is a root node.
     *
     * @return parent node instance.
     */
    node parent() const;

    /**
     * Get the string value of a JSON string node.
     *
     * @exception orcus::json_document_error if the node is not of the string
     *                 type.
     *
     * @return string value.
     */
    pstring string_value() const;

    /**
     * Get the numeric value of a JSON number node.
     *
     * @exception orcus::json_document_error if the node is not of the number
     *                 type.
     *
     * @return numeric value.
     */
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

typedef json::detail::node_t json_node_t;

/**
 * This class stores a parsed JSON document tree structure.
 */
class ORCUS_DLLPUBLIC json_document_tree
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    typedef json::detail::node node;

    json_document_tree();
    json_document_tree(string_pool& pool);
    ~json_document_tree();

    /**
     * Load raw string stream containing a JSON structure to populate the
     * document tree.
     *
     * @param strm stream containing a JSON structure.
     * @param config configuration object.
     */
    void load(const std::string& strm, const json_config& config);

    /**
     * Load raw string stream containing a JSON structure to populate the
     * document tree.
     *
     * @param p pointer to the stream containing a JSON structure.
     * @param n size of the stream.
     * @param config configuration object.
     */
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
