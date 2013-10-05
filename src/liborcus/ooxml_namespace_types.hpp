/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_OOXML_NAMESPACE_TYPES_HPP__
#define __ORCUS_OOXML_NAMESPACE_TYPES_HPP__

#include "orcus/types.hpp"

namespace orcus {

extern const xmlns_id_t NS_ooxml_r;
extern const xmlns_id_t NS_ooxml_xlsx;

extern const xmlns_id_t NS_opc_ct;
extern const xmlns_id_t NS_opc_rel;

/**
 * Null-terminated array of all ooxml namespaces.
 */
extern const xmlns_id_t* NS_ooxml_all;

/**
 * Null-terminated array of all opc namespaces.
 */
extern const xmlns_id_t* NS_opc_all;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
