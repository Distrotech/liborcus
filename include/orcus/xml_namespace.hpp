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

#include <ostream>

namespace orcus {

class xmlns_context;
class pstring;
struct xmlns_repository_impl;
struct xmlns_context_impl;

/**
 * Central XML namespace repository that stores all namespaces that are used
 * in the current session.
 */
class xmlns_repository
{
    friend class xmlns_context;
    xmlns_id_t intern(const pstring& uri);

    xmlns_repository(const xmlns_repository&); // disabled
    xmlns_repository& operator= (const xmlns_repository&); // disabled

    size_t get_index(xmlns_id_t ns_id) const;

public:
    ORCUS_DLLPUBLIC xmlns_repository();
    ORCUS_DLLPUBLIC ~xmlns_repository();

    ORCUS_DLLPUBLIC xmlns_context create_context();

    /**
     * Get XML namespace identifier from its numerical index.
     *
     * @param index numeric index of namespace.
     *
     * @return valid namespace identifier, or XMLNS_UNKNOWN_ID if not found.
     */
    ORCUS_DLLPUBLIC xmlns_id_t get_identifier(size_t index) const;

private:
    xmlns_repository_impl* mp_impl;
};

/**
 * XML namespace context.  A new context should be used for each xml stream
 * since the namespace keys themselves are not interned.  Don't hold an
 * instance of this class any longer than the life cycle of the xml stream
 * it is used in.
 *
 * An empty key value is associated with a default namespace.
 */
class ORCUS_DLLPUBLIC xmlns_context
{
    friend class xmlns_repository;

    xmlns_context(); // disabled
    xmlns_context(xmlns_repository& repo);
public:
    static size_t index_not_found;

    xmlns_context(const xmlns_context& r);
    ~xmlns_context();

    xmlns_id_t push(const pstring& key, const pstring& uri);
    void pop(const pstring& key);
    xmlns_id_t get(const pstring& key) const;
    size_t get_index(xmlns_id_t ns_id) const;
    void get_all_namespaces(std::vector<xmlns_id_t>& nslist) const;

    void dump(std::ostream& os) const;

private:
    xmlns_context_impl* mp_impl;
};

}

#endif
