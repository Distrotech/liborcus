/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_MEASUREMENT_HPP
#define ORCUS_MEASUREMENT_HPP

#include "types.hpp"
#include "env.hpp"

#include <cstdlib>
#include <string>

namespace orcus {

class pstring;

ORCUS_DLLPUBLIC double to_double(const char* p, const char* p_end, const char** p_parse_ended = nullptr);
ORCUS_DLLPUBLIC double to_double(const pstring& s);
ORCUS_DLLPUBLIC long to_long(const char* p, const char* p_end, const char** p_parse_ended = nullptr);
ORCUS_DLLPUBLIC long to_long(const pstring& s);
ORCUS_DLLPUBLIC bool to_bool(const pstring& s);

/**
 * Parse a string value containing a part representing a numerical value
 * optionally followed by a part representing a unit of measurement.
 *
 * Examples of such string value are: "1.234in", "0.34cm" and so on.
 *
 * @param str original string value.
 *
 * @return structure containing a numerical value and a unit of measurement
 *         that the original string value represents.
 */
ORCUS_DLLPUBLIC length_t to_length(const pstring& str);

ORCUS_DLLPUBLIC double convert(double value, length_unit_t unit_from, length_unit_t unit_to);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
