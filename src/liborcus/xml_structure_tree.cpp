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

#include "orcus/xml_structure_tree.hpp"
#include "orcus/sax_parser.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/global.hpp"
#include "orcus/exception.hpp"

#include "string_pool.hpp"

#include <iostream>
#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

namespace orcus {

namespace {

struct root
{
    xml_structure_tree::elem_name name;
    xml_structure_tree::elem_prop prop;
};

struct element_ref
{
    xml_structure_tree::elem_name name;
    xml_structure_tree::elem_prop* prop;

    element_ref(xml_structure_tree::elem_name _name, xml_structure_tree::elem_prop* _prop) :
        name(_name), prop(_prop) {}
};

typedef std::vector<element_ref> elements_type;

class xml_sax_handler
{
    xmlns_context& m_ns_cxt;
    string_pool& m_pool;
    unique_ptr<root> mp_root;
    elements_type m_stack;

public:
    xml_sax_handler(xmlns_context& ns_cxt, string_pool& pool) :
        m_ns_cxt(ns_cxt), m_pool(pool), mp_root(NULL) {}

    void declaration() {}

    void start_element(const sax_parser_element& elem)
    {
        xmlns_id_t ns_id = m_ns_cxt.get(elem.ns);
        if (!mp_root)
        {
            // This is a root element.
            mp_root.reset(new root);
            mp_root->name.ns = ns_id;
            mp_root->name.name = m_pool.intern(elem.name);
            element_ref ref(mp_root->name, &mp_root->prop);
            m_stack.push_back(ref);
            return;
        }

        // See if the current element already has a child element of the same name.
        assert(!m_stack.empty());
        const element_ref& current = m_stack.back();
        xml_structure_tree::elem_name key(ns_id, elem.name);
        xml_structure_tree::element_store_type::iterator it = current.prop->child_elements.find(key);
        if (it != current.prop->child_elements.end())
        {
            // Recurring element.
            it->second->repeat = true;
            element_ref ref(it->first, it->second);
            m_stack.push_back(ref);
            return;
        }

        // New element.
        key.name = m_pool.intern(key.name);
        pair<xml_structure_tree::element_store_type::iterator,bool> r =
            current.prop->child_elements.insert(
               xml_structure_tree::element_store_type::value_type(key, new xml_structure_tree::elem_prop));

        if (!r.second)
            throw general_error("Insertion failed");

        it = r.first;
        element_ref ref(it->first, it->second);
        m_stack.push_back(ref);
    }

    void end_element(const sax_parser_element& elem)
    {
        if (m_stack.empty())
            throw general_error("Element stack is empty.");

        const element_ref& current = m_stack.back();
        xmlns_id_t ns_id = m_ns_cxt.get(elem.ns);
        if (current.name.ns != ns_id || current.name.name != elem.name)
            throw general_error("Non-matching end element");

        m_stack.pop_back();
    }

    void characters(const pstring&) {}
    void attribute(const pstring&, const pstring&, const pstring&) {}

    root* release_root_element()
    {
        return mp_root.release();
    }
};

}

struct xml_structure_tree_impl
{
    string_pool m_pool;
    xmlns_repository& m_xmlns_repo;
    root* mp_root;

    xml_structure_tree_impl(xmlns_repository& xmlns_repo) :
        m_xmlns_repo(xmlns_repo), mp_root(NULL) {}

    ~xml_structure_tree_impl()
    {
        delete mp_root;
    }
};

xml_structure_tree::elem_name::elem_name() : ns(XMLNS_UNKNOWN_ID) {}

xml_structure_tree::elem_name::elem_name(xmlns_id_t _ns, const pstring& _name) :
    ns(_ns), name(_name) {}

xml_structure_tree::elem_name::elem_name(const elem_name& r) :
    ns(r.ns), name(r.name) {}

bool xml_structure_tree::elem_name::operator== (const elem_name& r) const
{
    return ns == r.ns && name == r.name;
}

size_t xml_structure_tree::elem_name::hash::operator ()(const xml_structure_tree::elem_name& val) const
{
    static pstring::hash hasher;
    size_t n = reinterpret_cast<size_t>(val.ns);
    n += hasher(val.name);
    return n;
}

xml_structure_tree::elem_prop::elem_prop() : repeat(false) {}

xml_structure_tree::elem_prop::~elem_prop()
{
    for_each(child_elements.begin(), child_elements.end(), map_object_deleter<element_store_type>());
}

xml_structure_tree::xml_structure_tree(xmlns_repository& xmlns_repo) :
    mp_impl(new xml_structure_tree_impl(xmlns_repo)) {}

xml_structure_tree::~xml_structure_tree()
{
    delete mp_impl;
}

void xml_structure_tree::parse(const char* p, size_t n)
{
    xmlns_context ns_cxt = mp_impl->m_xmlns_repo.create_context();
    xml_sax_handler hdl(ns_cxt, mp_impl->m_pool);
    sax_parser<xml_sax_handler> parser(p, n, hdl);
    parser.parse();
    mp_impl->mp_root = hdl.release_root_element();
}

namespace {

struct scope : boost::noncopyable
{
    xmlns_id_t ns;
    pstring name;
    elements_type elements;
    elements_type::const_iterator current_pos;
    bool repeat:1;

    scope(xmlns_id_t _ns, const pstring& _name, bool _repeat, const element_ref& _elem) :
        ns(_ns), name(_name), repeat(_repeat)
    {
        elements.push_back(_elem);
        current_pos = elements.begin();
    }

    scope(xmlns_id_t _ns, const pstring& _name, bool _repeat) :
        ns(_ns), name(_name), repeat(_repeat) {}
};

typedef boost::ptr_vector<scope> scopes_type;

void print_scope(ostream& os, const scopes_type& scopes)
{
    if (scopes.empty())
        throw general_error("scope stack shouldn't be empty while dumping tree.");

    // Skip the first scope which is root.
    scopes_type::const_iterator it = scopes.begin(), it_end = scopes.end();
    for (++it; it != it_end; ++it)
    {
        os << "/" << it->name;
        if (it->repeat)
            os << "*";
    }
}

}

void xml_structure_tree::dump_compact(ostream& os) const
{
    if (!mp_impl->mp_root)
        return;

    scopes_type scopes;

    element_ref ref(mp_impl->mp_root->name, &mp_impl->mp_root->prop);
    scopes.push_back(new scope(XMLNS_UNKNOWN_ID, pstring(), false, ref));
    while (!scopes.empty())
    {
        bool new_scope = false;

        // Iterate through all elements in the current scope.
        scope& cur_scope = scopes.back();
        for (; cur_scope.current_pos != cur_scope.elements.end(); ++cur_scope.current_pos)
        {
            const element_ref& this_elem = *cur_scope.current_pos;
            print_scope(os, scopes);

            os << "/" << this_elem.name.name;
            if (this_elem.prop->repeat)
                os << "*";
            os << endl;

            const xml_structure_tree::element_store_type& child_elements = this_elem.prop->child_elements;
            if (child_elements.empty())
                continue;

            // This element has child elements.  Push a new scope and populate
            // it with all child elements.
            xml_structure_tree::element_store_type::const_iterator it = child_elements.begin(), it_end = child_elements.end();
            elements_type elems;
            for (; it != it_end; ++it)
            {
                ref.name = it->first;
                ref.prop = it->second;
                elems.push_back(ref);
            }

            assert(!elems.empty());

            // Push a new scope, and restart the loop with the new scope.
            ++cur_scope.current_pos;
            scopes.push_back(new scope(this_elem.name.ns, this_elem.name.name, this_elem.prop->repeat));
            scope& child_scope = scopes.back();
            child_scope.elements.swap(elems);
            child_scope.current_pos = child_scope.elements.begin();

            new_scope = true;
            break;
        }

        if (new_scope)
            continue;

        scopes.pop_back();
    }
}

}
