/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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

    /**
     * Add a set of predefined namespace values to the repository.
     *
     * @param predefined_ns predefined set of namespace values. This is a
     *                      null-terminated array of xmlns_id_t.  This
     *                      xmlns_repository instance will assume that the
     *                      instances of these xmlns_id_t values will be
     *                      available throughout its life cycle; caller needs
     *                      to ensure that they won't get deleted before the
     *                      corresponding xmlns_repository instance is
     *                      deleted.
     */
    ORCUS_DLLPUBLIC void add_predefined_values(const xmlns_id_t* predefined_ns);

    ORCUS_DLLPUBLIC xmlns_context create_context();

    /**
     * Get XML namespace identifier from its numerical index.
     *
     * @param index numeric index of namespace.
     *
     * @return valid namespace identifier, or XMLNS_UNKNOWN_ID if not found.
     */
    ORCUS_DLLPUBLIC xmlns_id_t get_identifier(size_t index) const;

    ORCUS_DLLPUBLIC std::string get_short_name(xmlns_id_t ns_id) const;
    ORCUS_DLLPUBLIC std::string get_short_name(size_t index) const;

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
    xmlns_context(const xmlns_context& r);
    ~xmlns_context();

    xmlns_id_t push(const pstring& key, const pstring& uri);
    void pop(const pstring& key);
    xmlns_id_t get(const pstring& key) const;
    size_t get_index(xmlns_id_t ns_id) const;
    std::string get_short_name(xmlns_id_t ns_id) const;
    void get_all_namespaces(std::vector<xmlns_id_t>& nslist) const;

    void dump(std::ostream& os) const;

private:
    xmlns_context_impl* mp_impl;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
