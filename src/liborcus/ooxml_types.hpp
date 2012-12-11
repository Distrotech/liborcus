/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
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

#ifndef __ORCUS_OOXML_TYPES_HPP__
#define __ORCUS_OOXML_TYPES_HPP__

#include "orcus/pstring.hpp"

#include <iostream>
#include <boost/unordered_map.hpp>

namespace orcus {

typedef const char* content_type_t;
typedef const char* schema_t;

/**
 * Part name (first) and content type (second).
 */
typedef ::std::pair<pstring, content_type_t>    xml_part_t;

/**
 * Single OPC relationship that corresponds with a Relationship element in
 * .rels parts.
 */
struct opc_rel_t
{
    pstring  rid;
    pstring  target;
    schema_t type;

    opc_rel_t() : type(NULL) {}
    opc_rel_t(const pstring& _rid, const pstring& _target, schema_t _type) :
        rid(_rid), target(_target), type(_type) {}
};

/**
 * Used only to allow custom data associated with a relationship.
 */
struct opc_rel_extra
{
    virtual ~opc_rel_extra() = 0;
};

inline opc_rel_extra::~opc_rel_extra() {}

typedef boost::unordered_map<pstring, const opc_rel_extra*, pstring::hash>
    opc_rel_extras_t;

}

#endif
