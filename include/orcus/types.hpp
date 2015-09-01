/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_TYPES_HPP
#define INCLUDED_ORCUS_TYPES_HPP

#include <cstdlib>
#include <vector>
#include <string>
#include "pstring.hpp"
#include "env.hpp"

namespace orcus {

// XML specific types

typedef size_t xml_token_t;
typedef const char* xmlns_id_t;

ORCUS_PSR_DLLPUBLIC extern const xmlns_id_t XMLNS_UNKNOWN_ID;
ORCUS_PSR_DLLPUBLIC extern const xml_token_t XML_UNKNOWN_TOKEN;
ORCUS_PSR_DLLPUBLIC extern const size_t index_not_found;
ORCUS_PSR_DLLPUBLIC extern const size_t unspecified;

struct xml_name_t
{
    xmlns_id_t ns;
    pstring name;

    xml_name_t() : ns(XMLNS_UNKNOWN_ID), name() {}
    xml_name_t(xmlns_id_t _ns, const pstring& _name) : ns(_ns), name(_name) {}
    xml_name_t(const xml_name_t& r) : ns(r.ns), name(r.name) {}
};

struct ORCUS_PSR_DLLPUBLIC xml_token_attr_t
{
    xmlns_id_t ns;
    xml_token_t name;
    pstring raw_name;
    pstring value;

    /**
     * Whether or not the attribute value is transient. A transient value is
     * not guaranteed to be valid after the start_element call ends.  A
     * non-transient value is guaranteed to be valid during the life cycle of
     * the xml stream it belongs to.
     */
    bool transient;

    xml_token_attr_t();
    xml_token_attr_t(
        xmlns_id_t _ns, xml_token_t _name, const pstring& _value, bool _transient);
    xml_token_attr_t(
        xmlns_id_t _ns, xml_token_t _name, const pstring& _raw_name,
        const pstring& _value, bool _transient);
};

// Other types

enum class length_unit_t
{
    unknown = 0,
    centimeter,
    millimeter,
    xlsx_column_digit,
    inch,
    point,
    twip,
    pixel

    // TODO: Add more.
};

struct ORCUS_PSR_DLLPUBLIC date_time_t
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;

    date_time_t();

    std::string to_string() const;
};

typedef ::std::vector<xml_token_attr_t> xml_attrs_t;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
