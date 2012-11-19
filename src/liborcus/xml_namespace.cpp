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

#include "orcus/xml_namespace.hpp"
#include "orcus/exception.hpp"
#include "string_pool.hpp"

#include <boost/unordered_map.hpp>
#include <vector>
#include <limits>

#define ORCUS_DEBUG_NAMESPACE_REPO 0

using namespace std;

#if ORCUS_DEBUG_NAMESPACE_REPO
#include <cstdio>
#include <iostream>
#endif

namespace orcus {

typedef boost::unordered_map<pstring, size_t, pstring::hash> strid_map_type;

struct xmlns_repository_impl
{
    string_pool m_pool; /// storage of live string instances.
    std::vector<pstring> m_identifiers; /// map strings to numerical identifiers.
    strid_map_type m_strid_map;
};

xmlns_repository::xmlns_repository() : mp_impl(new xmlns_repository_impl) {}

xmlns_repository::~xmlns_repository()
{
    delete mp_impl;
}

xmlns_id_t xmlns_repository::intern(const pstring& uri)
{
    if (uri.empty())
        return XMLNS_UNKNOWN_ID;

    try
    {
        std::pair<pstring, bool> r = mp_impl->m_pool.intern(uri);
        pstring uri_interned = r.first;
        if (!uri_interned.empty())
        {
            // Intern successful.
            if (r.second)
            {
                // This is a new instance. Assign a numerical identifier.
                mp_impl->m_strid_map.insert(
                    strid_map_type::value_type(r.first, mp_impl->m_identifiers.size()));
#if ORCUS_DEBUG_NAMESPACE_REPO
                cout << "xmlns_repository::intern: uri='" << uri_interned << "' (" << mp_impl->m_identifiers.size() << ")" << endl;
#endif
                mp_impl->m_identifiers.push_back(r.first);

                assert(mp_impl->m_pool.size() == mp_impl->m_identifiers.size());
                assert(mp_impl->m_pool.size() == mp_impl->m_strid_map.size());
            }
            return uri_interned.get();
        }
    }
    catch (const general_error&)
    {
    }

    return XMLNS_UNKNOWN_ID;
}

xmlns_context xmlns_repository::create_context()
{
    return xmlns_context(*this);
}

size_t xmlns_repository::get_index(xmlns_id_t ns_id) const
{
    if (!ns_id)
        return xmlns_context::index_not_found;

    strid_map_type::const_iterator it = mp_impl->m_strid_map.find(pstring(ns_id));
    if (it == mp_impl->m_strid_map.end())
        return xmlns_context::index_not_found;

    return it->second;
}

typedef std::vector<xmlns_id_t> xmlns_list_type;
typedef boost::unordered_map<pstring, xmlns_list_type, pstring::hash> alias_map_type;

struct xmlns_context_impl
{
    xmlns_repository& m_repo;
    xmlns_list_type m_all_ns; /// all namespaces ever used in this context.
    xmlns_list_type m_default;
    alias_map_type m_map;

    bool m_in_parse;

    xmlns_context_impl(xmlns_repository& repo) : m_repo(repo), m_in_parse(true) {}
    xmlns_context_impl(const xmlns_context_impl& r) :
        m_repo(r.m_repo), m_all_ns(r.m_all_ns), m_default(r.m_default), m_map(r.m_map), m_in_parse(r.m_in_parse) {}
};

size_t xmlns_context::index_not_found = std::numeric_limits<size_t>::max();

xmlns_context::xmlns_context(xmlns_repository& repo) : mp_impl(new xmlns_context_impl(repo)) {}
xmlns_context::xmlns_context(const xmlns_context& r) : mp_impl(new xmlns_context_impl(*r.mp_impl)) {}

xmlns_context::~xmlns_context()
{
    delete mp_impl;
}

xmlns_id_t xmlns_context::push(const pstring& key, const pstring& uri)
{
    if (!mp_impl->m_in_parse)
        throw general_error("You can only push or pop during parsing.");

#if ORCUS_DEBUG_NAMESPACE_REPO
    cout << "xmlns_context::push: key='" << key << "', uri='" << uri << "'" << endl;
#endif
    if (uri.empty())
        return XMLNS_UNKNOWN_ID;

    pstring uri_interned = mp_impl->m_repo.intern(uri);

    if (key.empty())
    {
        // empty key value is associated with default namespace.
        mp_impl->m_default.push_back(uri_interned.get());
        mp_impl->m_all_ns.push_back(uri_interned.get());
        return mp_impl->m_default.back();
    }

    // See if this key already exists.
    alias_map_type::iterator it = mp_impl->m_map.find(key);
    if (it == mp_impl->m_map.end())
    {
        // This is the first time this key is used.
        xmlns_list_type nslist;
        nslist.push_back(uri_interned.get());
        mp_impl->m_all_ns.push_back(uri_interned.get());
        std::pair<alias_map_type::iterator,bool> r =
            mp_impl->m_map.insert(alias_map_type::value_type(key, nslist));

        if (!r.second)
            // insertion failed.
            throw general_error("Failed to insert new namespace.");

        return nslist.back();
    }

    // The key already exists.
    xmlns_list_type& nslist = it->second;
    nslist.push_back(uri_interned.get());
    mp_impl->m_all_ns.push_back(uri_interned.get());
    return nslist.back();
}

void xmlns_context::pop(const pstring& key)
{
    if (!mp_impl->m_in_parse)
        throw general_error("You can only push or pop during parsing.");

    if (key.empty())
    {
        // empty key value is associated with default namespace.
        if (mp_impl->m_default.empty())
            throw general_error("default namespace stack is empty.");

        mp_impl->m_default.pop_back();
        return;
    }

    // See if this key really exists.
    alias_map_type::iterator it = mp_impl->m_map.find(key);
    if (it == mp_impl->m_map.end())
        throw general_error("failed to find the key.");

    xmlns_list_type& nslist = it->second;
    if (nslist.empty())
        throw general_error("namespace stack for this key is empty.");

    nslist.pop_back();
}

xmlns_id_t xmlns_context::get(const pstring& key) const
{
    if (key.empty())
        return mp_impl->m_default.empty() ? XMLNS_UNKNOWN_ID : mp_impl->m_default.back();

    alias_map_type::const_iterator it = mp_impl->m_map.find(key);
    if (it == mp_impl->m_map.end())
        // Key not found.
        return XMLNS_UNKNOWN_ID;

    return it->second.empty() ? XMLNS_UNKNOWN_ID : it->second.back();
}

size_t xmlns_context::get_index(xmlns_id_t ns_id) const
{
    return mp_impl->m_repo.get_index(ns_id);
}

namespace {

#if ORCUS_DEBUG_NAMESPACE_REPO
struct print_ns : std::unary_function<xmlns_id_t, void>
{
    void operator() (xmlns_id_t ns_id) const
    {
        const char* p = ns_id;
        printf("%p: %s\n", p, p);
    }
};
#endif

}

void xmlns_context::get_all_namespaces(std::vector<xmlns_id_t>& nslist) const
{
#if ORCUS_DEBUG_NAMESPACE_REPO
    cout << "xmlns_context::get_all_namespaces: count=" << mp_impl->m_all_ns.size() << endl;
    std::for_each(mp_impl->m_all_ns.begin(), mp_impl->m_all_ns.end(), print_ns());
#endif
    if (mp_impl->m_in_parse)
    {
        mp_impl->m_in_parse = false;

        // Sort it and remove duplicate.
        std::sort(mp_impl->m_all_ns.begin(), mp_impl->m_all_ns.end());
        xmlns_list_type::iterator it_unique_end =
            std::unique(mp_impl->m_all_ns.begin(), mp_impl->m_all_ns.end());
        mp_impl->m_all_ns.erase(it_unique_end, mp_impl->m_all_ns.end());
    }

    nslist.assign(mp_impl->m_all_ns.begin(), mp_impl->m_all_ns.end());
}

void xmlns_context::dump(std::ostream& os) const
{
    vector<xmlns_id_t> nslist;
    get_all_namespaces(nslist);
    vector<xmlns_id_t>::const_iterator it = nslist.begin(), it_end = nslist.end();
    for (; it != it_end; ++it)
    {
        xmlns_id_t ns_id = *it;
        size_t num_id = get_index(ns_id);
        if (num_id == index_not_found)
            continue;

        os << "ns" << num_id << "=\"" << ns_id << '"' << endl;
    }
}

}
