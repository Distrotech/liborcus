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
#include "orcus/sax_ns_parser.hpp"
#include "orcus/xml_namespace.hpp"
#include "orcus/global.hpp"
#include "orcus/exception.hpp"

#include "string_pool.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

namespace orcus {

namespace {

struct elem_prop;
typedef boost::unordered_map<xml_structure_tree::entity_name, elem_prop*, xml_structure_tree::entity_name::hash> element_store_type;
typedef boost::unordered_set<xml_structure_tree::entity_name, xml_structure_tree::entity_name::hash> attribute_names_type;

/** Element properties. */
struct elem_prop : boost::noncopyable
{
    element_store_type child_elements;
    attribute_names_type attributes;

    /** Store child element names in order of appearance. */
    xml_structure_tree::entity_names_type child_element_names;

    /** Store attribute names in order of appearance. */
    xml_structure_tree::entity_names_type attribute_names;

    size_t appearance_order;

    size_t in_scope_count;

    /**
     * When true, this element is the base element of repeated structures.
     * This flag is set only with the base element; none of the child
     * elements below the base element have this flag set.
     */
    bool repeat:1;

    elem_prop() : appearance_order(0), in_scope_count(1), repeat(false) {}
    elem_prop(size_t _appearance_order) : appearance_order(_appearance_order), in_scope_count(1), repeat(false) {}
    ~elem_prop()
    {
        for_each(child_elements.begin(), child_elements.end(), map_object_deleter<element_store_type>());
    };
};

struct root
{
    xml_structure_tree::entity_name name;
    elem_prop prop;
};

struct element_ref
{
    xml_structure_tree::entity_name name;
    elem_prop* prop;

    element_ref() : prop(NULL) {}
    element_ref(xml_structure_tree::entity_name _name, elem_prop* _prop) :
        name(_name), prop(_prop) {}
};

typedef std::vector<element_ref> elements_type;

class xml_sax_handler
{
    string_pool& m_pool;
    unique_ptr<root> mp_root;
    elements_type m_stack;
    xml_structure_tree::entity_names_type m_attrs;

private:
    void merge_attributes(elem_prop& prop)
    {
        xml_structure_tree::entity_names_type::const_iterator it = m_attrs.begin(), it_end = m_attrs.end();
        for (; it != it_end; ++it)
        {
            if (prop.attributes.find(*it) == prop.attributes.end())
            {
                // New attribute.  Insert it.
                prop.attributes.insert(*it);
                prop.attribute_names.push_back(*it);
            }
        }

        m_attrs.clear();
    }

public:
    xml_sax_handler(string_pool& pool) :
        m_pool(pool), mp_root(NULL) {}

    void declaration()
    {
        m_attrs.clear();
    }

    void start_element(const sax_ns_parser_element& elem)
    {
        if (!mp_root)
        {
            // This is a root element.
            mp_root.reset(new root);
            mp_root->name.ns = elem.ns;
            mp_root->name.name = m_pool.intern(elem.name).first;
            element_ref ref(mp_root->name, &mp_root->prop);
            merge_attributes(mp_root->prop);
            m_stack.push_back(ref);
            return;
        }

        // See if the current element already has a child element of the same name.
        assert(!m_stack.empty());
        element_ref& current = m_stack.back();
        xml_structure_tree::entity_name key(elem.ns, elem.name);
        element_store_type::const_iterator it = current.prop->child_elements.find(key);
        if (it != current.prop->child_elements.end())
        {
            // Recurring element. Set its repeat flag only when it occurs
            // multiple times in the same scope.
            ++it->second->in_scope_count;
            if (it->second->in_scope_count > 1)
                it->second->repeat = true;

            element_ref ref(it->first, it->second);
            merge_attributes(*it->second);
            m_stack.push_back(ref);
            return;
        }

        // New element.
        size_t order = current.prop->child_elements.size();
        key.name = m_pool.intern(key.name).first;
        pair<element_store_type::const_iterator,bool> r =
            current.prop->child_elements.insert(
                element_store_type::value_type(key, new elem_prop(order)));

        if (!r.second)
            throw general_error("Insertion failed");

        current.prop->child_element_names.push_back(key);

        it = r.first;
        element_ref ref(it->first, it->second);
        merge_attributes(*it->second);
        m_stack.push_back(ref);
    }

    void end_element(const sax_ns_parser_element& elem)
    {
        if (m_stack.empty())
            throw general_error("Element stack is empty.");

        const element_ref& current = m_stack.back();

        // Reset the in-scope count of all child elements to 0 before ending
        // the current scope.
        element_store_type::iterator it = current.prop->child_elements.begin(), it_end = current.prop->child_elements.end();
        for (; it != it_end; ++it)
            it->second->in_scope_count = 0;

        m_stack.pop_back();
    }

    void characters(const pstring&) {}

    void attribute(const pstring&, const pstring&)
    {
        // Attribute for declaration. We don't handle this.
    }

    void attribute(const sax_ns_parser_attribute& attr)
    {
        m_attrs.push_back(xml_structure_tree::entity_name(attr.ns, attr.name));
    }

    root* release_root_element()
    {
        return mp_root.release();
    }
};

struct sort_by_appearance : std::binary_function<element_ref, element_ref, bool>
{
    bool operator() (const element_ref& left, const element_ref& right) const
    {
        return left.prop->appearance_order < right.prop->appearance_order;
    }
};

struct scope : boost::noncopyable
{
    xml_structure_tree::entity_name name;
    elements_type elements;
    elements_type::const_iterator current_pos;
    bool repeat:1;

    scope(const xml_structure_tree::entity_name& _name, bool _repeat, const element_ref& _elem) :
        name(_name), repeat(_repeat)
    {
        elements.push_back(_elem);
        current_pos = elements.begin();
    }

    scope(const xml_structure_tree::entity_name& _name, bool _repeat) :
        name(_name), repeat(_repeat) {}
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
        os << "/" << it->name.name;
        if (it->repeat)
            os << "[*]";
    }
}

}

struct xml_structure_tree_impl : boost::noncopyable
{
    string_pool m_pool;
    xmlns_context& m_xmlns_cxt;
    root* mp_root;

    xml_structure_tree_impl(xmlns_context& xmlns_cxt) :
        m_xmlns_cxt(xmlns_cxt), mp_root(NULL) {}

    ~xml_structure_tree_impl()
    {
        delete mp_root;
    }
};

struct xml_structure_tree::walker_impl : boost::noncopyable
{
    root* mp_root; /// Root element of the authoritative tree.

    element_ref m_cur_elem;
    std::vector<element_ref> m_scopes;

    walker_impl() : mp_root(NULL) {}
};

xml_structure_tree::entity_name::entity_name() :
    ns(XMLNS_UNKNOWN_ID) {}

xml_structure_tree::entity_name::entity_name(xmlns_id_t _ns, const pstring& _name) :
    ns(_ns), name(_name) {}

bool xml_structure_tree::entity_name::operator< (const entity_name& r) const
{
    if (ns != r.ns)
        return ns < r.ns;

    return name < r.name;
}

bool xml_structure_tree::entity_name::operator== (const entity_name& r) const
{
    return ns == r.ns && name == r.name;
}

size_t xml_structure_tree::entity_name::hash::operator() (const entity_name& val) const
{
    static pstring::hash hasher;
    size_t n = reinterpret_cast<size_t>(val.ns);
    n += hasher(val.name);
    return n;
}

xml_structure_tree::element::element() :
    repeat(false) {}

xml_structure_tree::element::element(const entity_name& _name, bool _repeat) :
    name(_name), repeat(_repeat) {}

xml_structure_tree::walker::walker(const xml_structure_tree_impl& parent_impl) :
    mp_impl(new walker_impl)
{
    mp_impl->mp_root = parent_impl.mp_root;
}

xml_structure_tree::walker::walker(const walker& r) :
    mp_impl(new walker_impl)
{
    mp_impl->mp_root = r.mp_impl->mp_root;
}

xml_structure_tree::walker::~walker()
{
    delete mp_impl;
}

xml_structure_tree::walker& xml_structure_tree::walker::operator= (const walker& r)
{
    mp_impl->mp_root = r.mp_impl->mp_root;
    return *this;
}

xml_structure_tree::element xml_structure_tree::walker::root()
{
    if (!mp_impl->mp_root)
        throw general_error("Tree is empty.");

    mp_impl->m_scopes.clear();

    // Set the current element to root.
    element_ref ref(mp_impl->mp_root->name, &mp_impl->mp_root->prop);
    mp_impl->m_cur_elem = ref;
    mp_impl->m_scopes.push_back(ref);
    return xml_structure_tree::element(ref.name, false);
}

xml_structure_tree::element xml_structure_tree::walker::descend(const entity_name& name)
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    assert(mp_impl->m_scopes.back().prop);
    const element_store_type& child_elems = mp_impl->m_scopes.back().prop->child_elements;
    element_store_type::const_iterator it = child_elems.find(name);

    if (it == child_elems.end())
        throw general_error("Specified child element does not exist.");

    // Push this new child element onto the stack.
    element_ref ref(name, it->second);
    mp_impl->m_scopes.push_back(ref);

    return element(name, it->second->repeat);
}

xml_structure_tree::element xml_structure_tree::walker::ascend()
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    if (mp_impl->m_scopes.size() == 1)
        throw general_error("You can't ascend from the root element.");

    mp_impl->m_scopes.pop_back();
    const element_ref& ref = mp_impl->m_scopes.back();
    return element(ref.name, ref.prop->repeat);
}

void xml_structure_tree::walker::get_children(entity_names_type& names)
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    assert(mp_impl->m_scopes.back().prop);
    const elem_prop& prop = *mp_impl->m_scopes.back().prop;
    names.assign(prop.child_element_names.begin(), prop.child_element_names.end());
}

void xml_structure_tree::walker::get_attributes(entity_names_type& names)
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    assert(mp_impl->m_scopes.back().prop);
    const elem_prop& prop = *mp_impl->m_scopes.back().prop;
    names.assign(prop.attribute_names.begin(), prop.attribute_names.end());
}

xml_structure_tree::xml_structure_tree(xmlns_context& xmlns_cxt) :
    mp_impl(new xml_structure_tree_impl(xmlns_cxt)) {}

xml_structure_tree::~xml_structure_tree()
{
    delete mp_impl;
}

void xml_structure_tree::parse(const char* p, size_t n)
{
    xml_sax_handler hdl(mp_impl->m_pool);
    sax_ns_parser<xml_sax_handler> parser(p, n, mp_impl->m_xmlns_cxt, hdl);
    parser.parse();
    mp_impl->mp_root = hdl.release_root_element();
}

void xml_structure_tree::dump_compact(ostream& os) const
{
    if (!mp_impl->mp_root)
        return;

    scopes_type scopes;
    const xmlns_context& cxt = mp_impl->m_xmlns_cxt;

    // Dump all namespaces first.
    {
        // Defalut namespace
        vector<xmlns_id_t> nslist;
        cxt.get_all_namespaces(nslist);
        vector<xmlns_id_t>::const_iterator it = nslist.begin(), it_end = nslist.end();
        for (; it != it_end; ++it)
        {
            xmlns_id_t ns_id = cxt.get(*it);
            cout << "ns_id=" << ns_id << endl;
            size_t num_id = cxt.get_index(ns_id);
            os << "ns" << num_id << "=" << ns_id << endl;
        }
    }

    element_ref ref(mp_impl->mp_root->name, &mp_impl->mp_root->prop);
    scopes.push_back(new scope(entity_name(), false, ref));
    while (!scopes.empty())
    {
        bool new_scope = false;

        // Iterate through all elements in the current scope.
        scope& cur_scope = scopes.back();
        for (; cur_scope.current_pos != cur_scope.elements.end(); ++cur_scope.current_pos)
        {
            const element_ref& this_elem = *cur_scope.current_pos;
            ostringstream ss;
            print_scope(ss, scopes);

            ss << "/" << this_elem.name.name;
            if (this_elem.prop->repeat)
                ss << "[*]";

            string elem_name = ss.str();
            os << elem_name << endl;

            // Print all attributes that belong to this element.
            {
                const entity_names_type& attrs = this_elem.prop->attribute_names;
                entity_names_type::const_iterator it = attrs.begin(), it_end = attrs.end();
                for (; it != it_end; ++it)
                    os << elem_name << '@' << it->name << endl;
            }

            const element_store_type& child_elements = this_elem.prop->child_elements;
            if (child_elements.empty())
                continue;

            // This element has child elements.  Push a new scope and populate
            // it with all child elements.
            element_store_type::const_iterator it = child_elements.begin(), it_end = child_elements.end();
            elements_type elems;
            for (; it != it_end; ++it)
            {
                ref.name = it->first;
                ref.prop = it->second;
                elems.push_back(ref);
            }

            // Sort the elements by order of appearance.
            std::sort(elems.begin(), elems.end(), sort_by_appearance());

            assert(!elems.empty());

            // Push a new scope, and restart the loop with the new scope.
            ++cur_scope.current_pos;
            scopes.push_back(new scope(this_elem.name, this_elem.prop->repeat));
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

xml_structure_tree::walker xml_structure_tree::get_walker() const
{
    return walker(*mp_impl);
}

}
