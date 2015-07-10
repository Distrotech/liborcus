/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_BASE_HPP
#define INCLUDED_ORCUS_JSON_PARSER_BASE_HPP

#include "orcus/parser_base.hpp"

namespace orcus { namespace json {

class ORCUS_PSR_DLLPUBLIC parser_base : public ::orcus::parser_base
{
protected:
    parser_base(const char* p, size_t n);
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
