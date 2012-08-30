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

#ifndef __ORCUS_DOM_TREE_HPP__
#define __ORCUS_DOM_TREE_HPP__

#include "pstring.hpp"

#include <vector>
#include <ostream>

#include <boost/ptr_container/ptr_vector.hpp>

namespace orcus {

class ORCUS_DLLPUBLIC dom_tree
{
    dom_tree(const dom_tree&); // disabled
public:
    struct attr
    {
        pstring ns;
        pstring name;
        pstring value;

        attr(const pstring& _ns, const pstring& _name, const pstring& _value);
    };

    typedef std::vector<attr> attrs_type;

    enum node_type { node_element, node_content };

    struct node
    {
        node_type type;

        node(node_type _type) : type(_type) {}

        virtual ~node() = 0;
        virtual void print(std::ostream& os) const = 0;
    };

    typedef boost::ptr_vector<node> nodes_type;

    struct element : public node
    {
        pstring    ns;
        pstring    name;
        attrs_type attrs;
        nodes_type child_nodes;

        element(const pstring& _ns, const pstring& _name);
        virtual void print(std::ostream& os) const;
        virtual ~element();
    };

    typedef std::vector<element*> element_stack_type;

    struct content : public node
    {
        pstring value;

        content(const pstring& _value);
        virtual void print(std::ostream& os) const;
        virtual ~content();
    };

    dom_tree();
    ~dom_tree();

    void end_declaration();
    void start_element(const pstring& ns, const pstring& name);
    void end_element(const pstring& ns, const pstring& name);
    void set_characters(const pstring& val);
    void set_attribute(const pstring& ns, const pstring& name, const pstring& val);

    void dump_compact(std::ostream& os) const;

private:
    attrs_type m_doc_attrs;
    attrs_type m_cur_attrs;
    element_stack_type m_elem_stack;
    element* m_root;
};

std::ostream& operator<< (std::ostream& os, const dom_tree::attr& at);
std::ostream& operator<< (std::ostream& os, const dom_tree::node& nd);

}

#endif
