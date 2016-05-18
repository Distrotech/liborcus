/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_THREAD_HPP
#define INCLUDED_ORCUS_JSON_PARSER_THREAD_HPP

#include "orcus/env.hpp"

#include <memory>

namespace orcus { namespace json {

class ORCUS_PSR_DLLPUBLIC parser_thread
{
    struct impl;
    std::unique_ptr<impl> mp_impl;

public:
    parser_thread(const char* p, size_t n);
    ~parser_thread();

    void start();
};

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
