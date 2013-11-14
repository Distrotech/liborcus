/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_TYPES_HPP__
#define __ORCUS_TYPES_HPP__

#include <cstdlib>
#include <vector>
#include <string>
#include "pstring.hpp"
#include "env.hpp"

namespace orcus {

// XML specific types

typedef size_t xml_token_t;
typedef const char* xmlns_id_t;

ORCUS_DLLPUBLIC extern const xmlns_id_t XMLNS_UNKNOWN_ID;
ORCUS_DLLPUBLIC extern const xml_token_t XML_UNKNOWN_TOKEN;
ORCUS_DLLPUBLIC extern const size_t index_not_found;
ORCUS_DLLPUBLIC extern const size_t unspecified;

struct xml_name_t
{
    xmlns_id_t ns;
    pstring name;

    xml_name_t() : ns(XMLNS_UNKNOWN_ID), name() {}
    xml_name_t(xmlns_id_t _ns, const pstring& _name) : ns(_ns), name(_name) {}
    xml_name_t(const xml_name_t& r) : ns(r.ns), name(r.name) {}
};

struct xml_token_attr_t
{
    xmlns_id_t ns;
    xml_token_t name;
    pstring value;

    /**
     * Whether or not the attribute value is transient. A transient value is
     * not guaranteed to be valid after the start_element call ends.  A
     * non-transient value is guaranteed to be valid during the life cycle of
     * the xml stream it belongs to.
     */
    bool transient;

    xml_token_attr_t() : ns(XMLNS_UNKNOWN_ID), name(XML_UNKNOWN_TOKEN) {}
    xml_token_attr_t(xmlns_id_t _ns, xml_token_t _name, const pstring& _value, bool _transient) :
        ns(_ns), name(_name), value(_value), transient(_transient) {}
};

// Other types

enum length_unit_t
{
    length_unit_unknown = 0,
    length_unit_centimeter,
    length_unit_xlsx_column_digit,
    length_unit_inch,
    length_unit_point,
    length_unit_twip

    // TODO: Add more.
};

struct date_time_t
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;

    date_time_t() : year(0), month(0), day(0), hour(0), minute(0), second(0.0) {}
};

typedef ::std::vector<xml_token_attr_t> xml_attrs_t;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
