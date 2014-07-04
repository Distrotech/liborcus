/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ORCUS_OOXML_GLOBAL_HPP
#define ORCUS_OOXML_GLOBAL_HPP

#include "orcus/types.hpp"
#include "ooxml_types.hpp"

#include <string>
#include <functional>

namespace orcus {

struct opc_rel_t;
struct xml_token_attr_t;

/**
 * Function object to print relationship information.
 */
struct print_opc_rel : ::std::unary_function<opc_rel_t, void>
{
    void operator() (const opc_rel_t& v) const;
};

/**
 * Given a directory path and a file name, return a full path that combines
 * the two while resolving any parent directory path ".." markers.
 *
 * @param dir_path directory path.  It can optionally start with a '/', but
 *                 it must end with a '/'.
 * @param file_name file name.
 *
 * @return full file path.
 */
std::string resolve_file_path(const std::string& dir_path, const std::string& file_name);

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
