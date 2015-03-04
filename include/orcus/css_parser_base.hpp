/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_CSS_PARSER_BASE_HPP
#define INCLUDED_CSS_PARSER_BASE_HPP

#include "orcus/env.hpp"
#include <string>

namespace orcus { namespace css {

class ORCUS_PSR_DLLPUBLIC parse_error : public std::exception
{
    std::string m_msg;
public:
    parse_error(const std::string& msg);
    virtual ~parse_error() throw();
    virtual const char* what() const throw();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
