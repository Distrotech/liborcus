/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_OOXML_GLOBAL_HPP__
#define __ORCUS_OOXML_GLOBAL_HPP__

#include "orcus/types.hpp"
#include "ooxml_types.hpp"

#include <functional>

namespace orcus {

struct opc_rel_t;
struct xml_token_attr_t;

/**
 * Function object to print relationship information.
 */
struct print_opc_rel : ::std::unary_function<opc_rel_t, void>
{
    void operator() (const opc_rel_t& v) const;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
