/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_STREAM_HPP
#define INCLUDED_ORCUS_STREAM_HPP

#include "env.hpp"

#include <string>

namespace orcus {

/**
 * Load the content of a file into a file stream.
 *
 * @param filepath file to open
 * @return content of the file
 */
ORCUS_PSR_DLLPUBLIC std::string load_file_content(const char* filepath);

/**
 * Generate a sensible error output for parse error including the line where
 * the error occurred and the offset of the error position on that line.
 *
 * @param strm entire character stream where the error occurred.
 * @param offset offset of the error position within the stream.
 *
 * @return ORCUS_PSR_DLLPUBLIC std::string
 */
ORCUS_PSR_DLLPUBLIC std::string create_parse_error_output(
    const std::string& strm, std::ptrdiff_t offset);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
