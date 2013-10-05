/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "detection_result.hpp"

namespace orcus {

detection_result::detection_result(bool result) : m_result(result) {}

bool detection_result::get_result() const
{
    return m_result;
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
