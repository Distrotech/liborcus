/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XML_STRUCTURE_TREE_HPP__
#define __ORCUS_XML_STRUCTURE_TREE_HPP__

#include "env.hpp"
#include "types.hpp"

#include <ostream>

namespace orcus {

class xmlns_context;
struct xml_structure_tree_impl;

/**
 * Tree representing the structure of elements in XML content.  Recurring
 * elements under the same parent are represented by a single element
 * instance.  This tree only includes elements; no attributes and content
 * nodes appear in this tree.
 */
class ORCUS_DLLPUBLIC xml_structure_tree
{
    xml_structure_tree(const xml_structure_tree&); // disabled;
    xml_structure_tree& operator= (const xml_structure_tree&); // disabled

public:

    struct ORCUS_DLLPUBLIC entity_name
    {
        xmlns_id_t ns;
        pstring name;

        entity_name();
        entity_name(xmlns_id_t _ns, const pstring& _name);

        bool operator< (const entity_name& r) const;
        bool operator== (const entity_name& r) const;

        struct ORCUS_DLLPUBLIC hash
        {
            size_t operator ()(const entity_name& val) const;
        };
    };

    typedef std::vector<entity_name> entity_names_type;

    struct ORCUS_DLLPUBLIC element
    {
        entity_name name;
        bool repeat;

        element();
        element(const entity_name& _name, bool _repeat);
    };

    struct walker_impl;

    /**
     * This class allows client to traverse the tree.
     */
    class ORCUS_DLLPUBLIC walker
    {
        friend class xml_structure_tree;
        walker_impl* mp_impl;

        walker(); // disabled
        walker(const xml_structure_tree_impl& parent_impl);
    public:
        walker(const walker& r);
        ~walker();
        walker& operator= (const walker& r);

        /**
         * Set current position to the root element, and return the root
         * element.
         *
         * @return root element.
         */
        element root();

        /**
         * Descend into specified child element.
         *
         * @param ns namespace of child element
         * @param name name of child element
         *
         * @return child element
         */
        element descend(const entity_name& name);

        /**
         * Move up to the parent element.
         */
        element ascend();

        /**
         * Get a list of names of all child elements at current element
         * position.  The list of names is in order of appearance.
         *
         * @param names list of child element names in order of appearance.
         */
        void get_children(entity_names_type& names);

        /**
         * Get a list of names of all attributes that belong to current
         * element.  The list of names is in order of appearance.
         *
         * @param names list of attribute names in order of appearance.
         */
        void get_attributes(entity_names_type& names);

        /**
         * Get a numerical, 0-based index of given XML namespace.
         *
         * @param ns XML namespace ID.
         *
         * @return numeric, 0-based index of XML namespace if found, or
         *         <code>xml_structure_tree::walker::index_not_found</code> if
         *         the namespace is not found in this structure.
         */
        size_t get_xmlns_index(xmlns_id_t ns) const;

        std::string get_xmlns_short_name(xmlns_id_t ns) const;
    };

    xml_structure_tree(xmlns_context& xmlns_cxt);
    ~xml_structure_tree();

    void parse(const char* p, size_t n);

    void dump_compact(std::ostream& os) const;

    walker get_walker() const;

private:
    xml_structure_tree_impl* mp_impl;
};

}



#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
