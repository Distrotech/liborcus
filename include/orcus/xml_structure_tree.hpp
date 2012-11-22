/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

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

    struct entity_name
    {
        xmlns_id_t ns;
        pstring name;

        entity_name();
        entity_name(xmlns_id_t _ns, const pstring& _name);

        bool operator< (const entity_name& r) const;
        bool operator== (const entity_name& r) const;

        struct hash
        {
            size_t operator ()(const entity_name& val) const;
        };
    };

    typedef std::vector<entity_name> entity_names_type;

    struct element
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
    class walker
    {
        friend class xml_structure_tree;
        walker_impl* mp_impl;

        walker(); // disabled
        walker(const xml_structure_tree_impl& parent_impl);
    public:

        static size_t index_not_found;

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
