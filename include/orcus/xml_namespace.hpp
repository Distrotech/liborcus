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

#ifndef __ORCUS_XML_NAMESPACE_MANAGER_HPP__
#define __ORCUS_XML_NAMESPACE_MANAGER_HPP__

#include "types.hpp"
#include "string_pool.hpp"
#include "pstring.hpp"

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

namespace orcus {

class xmlns_context;

/**
 * Central XML namespace repository that stores all namespaces that are used
 * in the current session.
 */
class xmlns_repository : boost::noncopyable
{
public:
    xmlns_repository();
    ~xmlns_repository();

    xmlns_id_t intern(const pstring& uri);

    xmlns_context create_context();

private:
    string_pool m_pool;
};

/**
 * XML namespace context.  A new context should be used for each xml stream
 * since the namespace keys themselves are not interned.  Don't hold an
 * instance of this class any longer than the life cycle of the xml stream
 * it is used in.
 *
 * An empty key value is associated with a default namespace.
 */
class xmlns_context
{
    friend class xmlns_repository;

    typedef boost::unordered_map<pstring, pstring, pstring::hash> map_type;

    xmlns_context(); // disabled
    xmlns_context(xmlns_repository& repo);
public:
    xmlns_context(const xmlns_context& r);

    xmlns_id_t set(const pstring& key, const pstring& uri);
    xmlns_id_t get(const pstring& key) const;

private:
    xmlns_repository& m_repo;
    xmlns_id_t m_default;
    map_type m_map;
};

}

#endif
