/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_DUMPER_HPP
#define INCLUDED_ORCUS_JSON_DUMPER_HPP

#include <string>

namespace orcus { namespace spreadsheet {

class document;

namespace detail {

class json_dumper
{
    const document& m_doc;

public:
    json_dumper(const document& doc);

    void dump(const std::string& filepath) const;
};

}}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
