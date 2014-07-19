/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/interface.hpp"

namespace orcus { namespace iface {

config::config() : debug(false) {}

struct import_filter::impl
{
    config m_config;
};

import_filter::import_filter() : mp_impl(new impl) {}

import_filter::~import_filter()
{
    delete mp_impl;
}

void import_filter::set_config(const config& v)
{
    mp_impl->m_config = v;
}

const config& import_filter::get_config() const
{
    return mp_impl->m_config;
}

document_dumper::~document_dumper() {}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
