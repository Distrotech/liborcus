/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ooxml_global.hpp"
#include "ooxml_types.hpp"
#include "ooxml_token_constants.hpp"

#include <iostream>

using namespace std;

namespace orcus {

void print_opc_rel::operator() (const opc_rel_t& v) const
{
    using namespace std;
    cout << v.rid << ": " << v.target << " (" << v.type << ")" << endl;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
