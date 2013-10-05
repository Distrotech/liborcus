/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_PARSER_GLOBAL_HPP
#define ORCUS_PARSER_GLOBAL_HPP

#include "env.hpp"

namespace orcus {

ORCUS_DLLPUBLIC bool is_blank(char c);
ORCUS_DLLPUBLIC bool is_alpha(char c);
ORCUS_DLLPUBLIC bool is_name_char(char c);
ORCUS_DLLPUBLIC bool is_numeric(char c);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
