/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_JSON_PARSER_HPP
#define INCLUDED_ORCUS_JSON_PARSER_HPP

#include "orcus/json_parser_base.hpp"

namespace orcus {

template<typename _Handler>
class json_parser
{
public:
    typedef _Handler handler_type;

    json_parser(const char* p, size_t n, handler_type& hdl);

    void parse();

private:
    handler_type& m_handler;
};

template<typename _Handler>
json_parser<_Handler>::csv_parser(
    const char* p, size_t n, handler_type& hdl) :
    json::parser_base(p, n), m_handler(hdl) {}

template<typename _Handler>
void csv_parser<_Handler>::parse()
{
    m_handler.begin_parse();
    m_handler.end_parse();
}

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
