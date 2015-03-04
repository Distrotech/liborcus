/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/css_parser_base.hpp"

namespace orcus { namespace css {

parse_error::parse_error(const std::string& msg) : m_msg(msg) {}
parse_error::~parse_error() throw() {}
const char* parse_error::what() const throw() { return m_msg.c_str(); }

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
