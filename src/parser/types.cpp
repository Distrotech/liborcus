/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/types.hpp"

#include <limits>
#include <sstream>

namespace orcus {

const xmlns_id_t XMLNS_UNKNOWN_ID = NULL;
const xml_token_t XML_UNKNOWN_TOKEN = 0;

const size_t index_not_found = std::numeric_limits<size_t>::max();
const size_t unspecified = std::numeric_limits<size_t>::max()-1;

date_time_t::date_time_t() :
    year(0), month(0), day(0), hour(0), minute(0), second(0.0) {}

std::string date_time_t::to_string() const
{
    std::ostringstream os;
    os << year << "-" << month << "-" << day << "T" << hour << ":" << minute << ":" << second;
    return os.str();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
