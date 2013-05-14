/*************************************************************************
 *
 * Copyright (c) 2010-2012 Kohei Yoshida
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

    xml_token_attr_t() : ns(XMLNS_UNKNOWN_ID), name(XML_UNKNOWN_TOKEN) {}
    xml_token_attr_t(xmlns_id_t _ns, xml_token_t _name, const pstring& _value) :
        ns(_ns), name(_name), value(_value) {}
};

// Other types

enum length_unit_t
{
    length_unit_unknown = 0,
    length_unit_centimeter,
    length_unit_excel_column,
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
