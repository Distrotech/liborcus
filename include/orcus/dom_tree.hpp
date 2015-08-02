/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_DOM_TREE_HPP
#define INCLUDED_ORCUS_DOM_TREE_HPP

#include "pstring.hpp"
#include "types.hpp"

#include <vector>
#include <ostream>
#include <memory>

namespace orcus {

class xmlns_context;
struct dom_tree_impl;

namespace sax {

struct doctype_declaration;

}

/**
 * Ordinary DOM tree representing the structure of a XML content in full.
 */
class ORCUS_DLLPUBLIC dom_tree
{
    dom_tree(const dom_tree&) = delete;
    dom_tree& operator= (const dom_tree&) = delete;

public:

    struct entity_name
    {
        xmlns_id_t ns;
        pstring name;

        entity_name();
        entity_name(xmlns_id_t _ns, const pstring& _name);

        void print(std::ostream& os, const xmlns_context& cxt) const;
    };

    struct attr
    {
        entity_name name;
        pstring value;

        attr(xmlns_id_t _ns, const pstring& _name, const pstring& _value);

        void print(std::ostream& os, const xmlns_context& cxt) const;
    };

    typedef std::vector<attr> attrs_type;

    enum class node_type { element, content };

    struct node
    {
        node_type type;

        node(node_type _type) : type(_type) {}

        virtual ~node() = 0;
        virtual void print(std::ostream& os, const xmlns_context& cxt) const = 0;
    };

    typedef std::vector<std::unique_ptr<node>> nodes_type;

    struct element : public node
    {
        entity_name name;
        attrs_type attrs;
        nodes_type child_nodes;

        element(xmlns_id_t _ns, const pstring& _name);
        virtual void print(std::ostream& os, const xmlns_context& cxt) const;
        virtual ~element();
    };

    typedef std::vector<element*> element_stack_type;

    struct content : public node
    {
        pstring value;

        content(const pstring& _value);
        virtual void print(std::ostream& os, const xmlns_context& cxt) const;
        virtual ~content();
    };

    dom_tree(xmlns_context& cxt);
    ~dom_tree();

    /**
     * Parse a given XML stream and build the content tree.
     *
     * @param strm XML stream.
     */
    void load(const std::string& strm);

    /**
     * Swap the content with another dom_tree instance.
     *
     * @param other the dom_tree instance to swap the content with.
     */
    void swap(dom_tree& other);

    void start_declaration(const pstring& name);
    void end_declaration(const pstring& name);
    void start_element(xmlns_id_t ns, const pstring& name);
    void end_element(xmlns_id_t ns, const pstring& name);
    void set_characters(const pstring& val);
    void set_attribute(xmlns_id_t ns, const pstring& name, const pstring& val);

    void set_doctype(const sax::doctype_declaration& dtd);
    const sax::doctype_declaration* get_doctype() const;

    const attrs_type* get_declaration_attributes(const pstring& name) const;

    void dump_compact(std::ostream& os) const;

private:
    std::unique_ptr<dom_tree_impl> mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
