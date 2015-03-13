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

ORCUS_PSR_DLLPUBLIC bool is_blank(char c);
ORCUS_PSR_DLLPUBLIC bool is_alpha(char c);
ORCUS_PSR_DLLPUBLIC bool is_name_char(char c);
ORCUS_PSR_DLLPUBLIC bool is_numeric(char c);

/**
 * Check if the characater is one of allowed characters.
 *
 * @param c character to check.
 * @param allowed null-terminated character array containing all allowed
 *                characters.
 *
 * @return true if the character is one of the allowed characters, false
 *         otherwise.
 */
ORCUS_PSR_DLLPUBLIC bool is_in(char c, const char* allowed);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
