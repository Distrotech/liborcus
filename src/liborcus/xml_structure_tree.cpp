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

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

namespace orcus {

namespace {

/** Element name. */
struct elem_name
{
    xmlns_id_t ns;
    pstring name;

    struct hash
    {
        size_t operator ()(const elem_name& val) const
        {
            static pstring::hash hasher;
            size_t n = reinterpret_cast<size_t>(val.ns);
            n += hasher(val.name);
            return n;
        }
    };

    elem_name() : ns(XMLNS_UNKNOWN_ID) {}
    elem_name(xmlns_id_t _ns, const pstring& _name) : ns(_ns), name(_name) {}
    elem_name(const elem_name& r) : ns(r.ns), name(r.name) {}

    bool operator== (const elem_name& r) const
    {
        return ns == r.ns && name == r.name;
    }
};

struct elem_prop;
typedef boost::unordered_map<elem_name, elem_prop*, elem_name::hash> element_store_type;

/** Element properties. */
struct elem_prop : boost::noncopyable
{
    element_store_type child_elements;

    /**
     * When true, this element is the base element of repeated structures.
     * This flag is set only with the base element; none of the child
     * elements below the base element have this flag set.
     */
    bool repeat:1;

    elem_prop() : repeat(false) {}
    ~elem_prop()
    {
        for_each(child_elements.begin(), child_elements.end(), map_object_deleter<element_store_type>());
    };
};

struct root
{
    elem_name name;
    elem_prop prop;
};

struct element_ref
{
    elem_name name;
    const elem_prop* prop;
    bool in_repeated_element:1;

    element_ref() : prop(NULL) {}
    element_ref(elem_name _name, const elem_prop* _prop) :
        name(_name), prop(_prop), in_repeated_element(false) {}
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
        elem_name key(ns_id, elem.name);
        element_store_type::const_iterator it = current.prop->child_elements.find(key);
        if (it != current.prop->child_elements.end())
        {
            // Recurring element.
            if (!current.in_repeated_element)
                // Set this flag only with the base element of repeated structures.
                it->second->repeat = true;
            element_ref ref(it->first, it->second);
            ref.in_repeated_element = true;
            m_stack.push_back(ref);
            return;
        }

        // New element.
        key.name = m_pool.intern(key.name);
        pair<element_store_type::const_iterator,bool> r =
            const_cast<elem_prop*>(current.prop)->child_elements.insert(
               element_store_type::value_type(key, new elem_prop));

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

struct sort_by_name : std::binary_function<element_ref, element_ref, bool>
{
    bool operator() (const element_ref& left, const element_ref& right) const
    {
        if (left.name.ns != right.name.ns)
            return left.name.ns < right.name.ns;

        return left.name.name < right.name.name;
    }
};

struct scope : boost::noncopyable
{
    elem_name name;
    elements_type elements;
    elements_type::const_iterator current_pos;
    bool repeat:1;

    scope(const elem_name& _name, bool _repeat, const element_ref& _elem) :
        name(_name), repeat(_repeat)
    {
        elements.push_back(_elem);
        current_pos = elements.begin();
    }

    scope(const elem_name& _name, bool _repeat) :
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
    xmlns_repository& m_xmlns_repo;
    root* mp_root;

    xml_structure_tree_impl(xmlns_repository& xmlns_repo) :
        m_xmlns_repo(xmlns_repo), mp_root(NULL) {}

    ~xml_structure_tree_impl()
    {
        delete mp_root;
    }
};

struct xml_structure_tree::walker_impl : boost::noncopyable
{
    const root* mp_root; /// Root element of the authoritative tree.

    element_ref m_cur_elem;
    std::vector<element_ref> m_scopes;

    walker_impl() : mp_root(NULL) {}
};

xml_structure_tree::element_name::element_name() :
    ns(XMLNS_UNKNOWN_ID) {}

xml_structure_tree::element_name::element_name(xmlns_id_t _ns, const pstring& _name) :
    ns(_ns), name(_name) {}

bool xml_structure_tree::element_name::operator< (const element_name& r) const
{
    if (ns != r.ns)
        return ns < r.ns;

    return name < r.name;
}

xml_structure_tree::element::element() :
    repeat(false) {}

xml_structure_tree::element::element(const element_name& _name, bool _repeat) :
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
    xml_structure_tree::element_name name(ref.name.ns, ref.name.name);
    return xml_structure_tree::element(name, false);
}

xml_structure_tree::element xml_structure_tree::walker::descend(const element_name& name)
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    assert(mp_impl->m_scopes.back().prop);
    const element_store_type& child_elems = mp_impl->m_scopes.back().prop->child_elements;
    element_store_type::const_iterator it = child_elems.find(elem_name(name.ns, name.name));

    if (it == child_elems.end())
        throw general_error("Specified child element does not exist.");

    // Push this new child element onto the stack.
    element_ref ref(elem_name(name.ns, name.name), it->second);
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
    return element(element_name(ref.name.ns, ref.name.name), ref.prop->repeat);
}

void xml_structure_tree::walker::get_children(element_names_type& names)
{
    if (mp_impl->m_scopes.empty())
        throw general_error("Scope is empty.");

    assert(mp_impl->m_scopes.back().prop);
    const elem_prop& prop = *mp_impl->m_scopes.back().prop;
    element_names_type _names;
    element_store_type::const_iterator it = prop.child_elements.begin(), it_end = prop.child_elements.end();
    for (; it != it_end; ++it)
    {
        const elem_name& name = it->first;
        _names.push_back(element_name(name.ns, name.name));
    }

    // Sort the names.
    sort(_names.begin(), _names.end());

    names.swap(_names);
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

void xml_structure_tree::dump_compact(ostream& os) const
{
    if (!mp_impl->mp_root)
        return;

    scopes_type scopes;

    element_ref ref(mp_impl->mp_root->name, &mp_impl->mp_root->prop);
    scopes.push_back(new scope(elem_name(), false, ref));
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
                os << "[*]";
            os << endl;

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

            // Sort the elements by name to make their order tractable.
            std::sort(elems.begin(), elems.end(), sort_by_name());

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
