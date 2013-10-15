/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_XLSX_SESSION_DATA_HPP
#define ORCUS_XLSX_SESSION_DATA_HPP

#include "session_context.hpp"

namespace orcus {

/**
 * Collection of global data that need to be persistent during a single
 * import session.
 */
struct xlsx_session_data : public session_context::custom_data
{
    virtual ~xlsx_session_data();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
