/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_tokens.hpp"

namespace orcus {

namespace {

#include "odf_tokens.inl"

}

tokens odf_tokens = tokens(token_names, token_name_count);

}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
