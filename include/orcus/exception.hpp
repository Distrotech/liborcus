/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_EXCEPTION_HPP__
#define __ORCUS_EXCEPTION_HPP__

#include <exception>
#include <string>

#include "env.hpp"

namespace orcus {

class ORCUS_DLLPUBLIC general_error : public ::std::exception
{
public:
    explicit general_error(const ::std::string& msg);
    virtual ~general_error() throw();
    virtual const char* what() const throw();
private:
    ::std::string m_msg;
};

class ORCUS_DLLPUBLIC xml_structure_error : public general_error
{
public:
    explicit xml_structure_error(const ::std::string& msg);
    virtual ~xml_structure_error() throw();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
