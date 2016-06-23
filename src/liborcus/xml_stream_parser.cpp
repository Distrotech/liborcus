/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xml_stream_parser.hpp"
#include "xml_stream_handler.hpp"

#include "orcus/tokens.hpp"

#ifdef __ORCUS_THREADED_XML_PARSING
#include "orcus/threaded_sax_token_parser.hpp"
#else
#include "orcus/sax_token_parser.hpp"
#endif

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

namespace orcus {

// ============================================================================

xml_stream_parser::parse_error::parse_error(const string& msg) :
    m_msg(msg) {}

xml_stream_parser::parse_error::~parse_error() throw() {}

const char* xml_stream_parser::parse_error::what() const throw()
{
    return m_msg.c_str();
}

xml_stream_parser::xml_stream_parser(
    const config& opt,
    xmlns_repository& ns_repo, const tokens& tokens, const char* content, size_t size) :
    m_config(opt),
    m_ns_cxt(ns_repo.create_context()),
    m_tokens(tokens),
    mp_handler(NULL),
    m_content(content),
    m_size(size)
{
}

xml_stream_parser::~xml_stream_parser()
{
}

void xml_stream_parser::parse()
{
    if (!mp_handler)
        return;

#ifdef __ORCUS_THREADED_XML_PARSING
    threaded_sax_token_parser<xml_stream_handler> sax(m_content, m_size, m_tokens, m_ns_cxt, *mp_handler, 1000);
#else
    sax_token_parser<xml_stream_handler> sax(m_content, m_size, m_tokens, m_ns_cxt, *mp_handler);
#endif

    sax.parse();
}

void xml_stream_parser::set_handler(xml_stream_handler* handler)
{
    mp_handler = handler;
    if (mp_handler)
    {
        mp_handler->set_ns_context(&m_ns_cxt);
        mp_handler->set_config(m_config);
    }
}

xml_stream_handler* xml_stream_parser::get_handler() const
{
    return mp_handler;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
