/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_XMLPARSER_HPP__
#define __ORCUS_XMLPARSER_HPP__

#include <cstdlib>
#include <string>
#include <exception>

#include "orcus/xml_namespace.hpp"

namespace orcus {

struct config;

class xml_stream_handler;
class tokens;

/**
 * This class does NOT store the stream content which is just a pointer to
 * the first char of the content stream.  Make sure you finish parsing while
 * the content pointer is valid.
 */
class xml_stream_parser_base
{
public:
    xml_stream_parser_base() = delete;

    virtual void parse() = 0;

    void set_handler(xml_stream_handler* handler);
    xml_stream_handler* get_handler() const;

protected:
    xml_stream_parser_base(
        const config& opt,
        xmlns_repository& ns_repo, const tokens& tokens, const char* content, size_t size);
    ~xml_stream_parser_base();

    const config& m_config;
    xmlns_context m_ns_cxt;
    const tokens& m_tokens;
    xml_stream_handler* mp_handler;
    const char* m_content;
    size_t m_size;
};

class xml_stream_parser : public xml_stream_parser_base
{
public:
    xml_stream_parser(
        const config& opt,
        xmlns_repository& ns_repo, const tokens& tokens, const char* content, size_t size);
    ~xml_stream_parser();

    virtual void parse();
};

class threaded_xml_stream_parser : public xml_stream_parser_base
{
public:
    threaded_xml_stream_parser(
        const config& opt,
        xmlns_repository& ns_repo, const tokens& tokens, const char* content, size_t size);
    ~threaded_xml_stream_parser();

    virtual void parse();
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
