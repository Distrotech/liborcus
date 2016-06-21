/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/types.hpp"

#include <limits>
#include <sstream>

namespace orcus {

const xmlns_id_t XMLNS_UNKNOWN_ID = NULL;
const xml_token_t XML_UNKNOWN_TOKEN = 0;

const size_t index_not_found = std::numeric_limits<size_t>::max();
const size_t unspecified = std::numeric_limits<size_t>::max()-1;

xml_token_attr_t::xml_token_attr_t() :
    ns(XMLNS_UNKNOWN_ID), name(XML_UNKNOWN_TOKEN), transient(false) {}

xml_token_attr_t::xml_token_attr_t(
    xmlns_id_t _ns, xml_token_t _name, const pstring& _value, bool _transient) :
    ns(_ns), name(_name), value(_value), transient(_transient) {}

xml_token_attr_t::xml_token_attr_t(
    xmlns_id_t _ns, xml_token_t _name, const pstring& _raw_name, const pstring& _value, bool _transient) :
    ns(_ns), name(_name), raw_name(_raw_name), value(_value), transient(_transient) {}

xml_token_element_t::xml_token_element_t() : ns(nullptr), name(XML_UNKNOWN_TOKEN) {}

xml_token_element_t::xml_token_element_t(
    xmlns_id_t _ns, xml_token_t _name, const pstring& _raw_name, std::vector<xml_token_attr_t>&& _attrs)  :
    ns(_ns), name(_name), raw_name(_raw_name), attrs(std::move(_attrs)) {}

xml_token_element_t::xml_token_element_t(xml_token_element_t&& other) :
    ns(other.ns), name(other.name), raw_name(other.raw_name), attrs(std::move(other.attrs)) {}

length_t::length_t() : unit(length_unit_t::unknown), value(0.0) {}

std::string length_t::print() const
{
    std::ostringstream os;
    os << value;

    switch (unit)
    {
        case length_unit_t::centimeter:
            os << " cm";
        break;
        case length_unit_t::inch:
            os << " in";
        break;
        case length_unit_t::point:
            os << " pt";
        break;
        case length_unit_t::twip:
            os << " twip";
        break;
        case length_unit_t::unknown:
        default:
            ;
    }

    return os.str();
}

date_time_t::date_time_t() :
    year(0), month(0), day(0), hour(0), minute(0), second(0.0) {}

std::string date_time_t::to_string() const
{
    std::ostringstream os;
    os << year << "-" << month << "-" << day << "T" << hour << ":" << minute << ":" << second;
    return os.str();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
