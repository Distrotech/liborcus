/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_tokens.hpp"

namespace orcus {

namespace ooxml {

#include "ooxml_tokens.inl"

}

namespace opc {

#include "opc_tokens.inl"

}

tokens ooxml_tokens = tokens(ooxml::token_names, ooxml::token_name_count);

tokens opc_tokens = tokens(opc::token_names, opc::token_name_count);

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
