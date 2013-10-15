/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "session_context.hpp"

namespace orcus {

session_context::custom_data::~custom_data() {}

session_context::session_context() : mp_data(NULL) {}
session_context::session_context(custom_data* data) : mp_data(data) {}

session_context::~session_context()
{
    mp_data.reset();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
