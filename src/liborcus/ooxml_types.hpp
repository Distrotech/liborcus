/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
