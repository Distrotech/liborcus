/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_DOM_TREE_HPP__
#define __ORCUS_DOM_TREE_HPP__

#include "pstring.hpp"
#include "types.hpp"

#include <vector>
#include <ostream>

#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

class xmlns_context;
struct dom_tree_impl;

/**
 * Ordinary DOM tree representing the structure of a XML content in full.
 */
class ORCUS_DLLPUBLIC dom_tree
{
    dom_tree(const dom_tree&); // disabled
    dom_tree& operator= (const dom_tree&); // disabled

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

    enum node_type { node_element, node_content };

    struct node
    {
        node_type type;

        node(node_type _type) : type(_type) {}

        virtual ~node() = 0;
        virtual void print(std::ostream& os, const xmlns_context& cxt) const = 0;
    };

    typedef boost::ptr_vector<node> nodes_type;

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

    void start_declaration(const pstring& name);
    void end_declaration(const pstring& name);
    void start_element(xmlns_id_t ns, const pstring& name);
    void end_element(xmlns_id_t ns, const pstring& name);
    void set_characters(const pstring& val);
    void set_attribute(xmlns_id_t ns, const pstring& name, const pstring& val);

    const attrs_type* get_declaration_attributes(const pstring& name) const;

    void dump_compact(std::ostream& os) const;

private:
    dom_tree_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
