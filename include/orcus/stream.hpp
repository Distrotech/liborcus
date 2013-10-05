/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_STREAM_HPP__
#define __ORCUS_STREAM_HPP__

#include "env.hpp"

#include <string>

namespace orcus {

/**
 * Load the content of a file into a file stream.
 *
 * @param filepath file to open
 * @param strm content of the file
 */
ORCUS_DLLPUBLIC void load_file_content(const char* filepath, std::string& strm);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
