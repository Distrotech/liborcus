/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_CONFIG_HPP
#define ORCUS_CONFIG_HPP

#include "env.hpp"

namespace orcus {

struct ORCUS_DLLPUBLIC config
{
    /**
     * Enable or disable runtime debug output to stdout or stderr.
     */
    bool debug;

    /**
     * Control whether or not to perform strict check of the xml structure of
     * a stream being parsed.  When enabled, it throws an xml_structure_error
     * exception when an incorrect xml structure is detected.
     */
    bool structure_check;

    config();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
