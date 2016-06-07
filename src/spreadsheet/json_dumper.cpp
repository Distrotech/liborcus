/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "json_dumper.hpp"

#include <fstream>
#include <iostream>

namespace orcus { namespace spreadsheet { namespace detail {

json_dumper::json_dumper(const document& doc) : m_doc(doc) {}

void json_dumper::dump(const std::string& filepath) const
{
    std::ofstream file(filepath.c_str());
    if (!file)
    {
        std::cerr << "failed to create file: " << filepath << std::endl;
        return;
    }

    // TODO : implement this.
}

}}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
