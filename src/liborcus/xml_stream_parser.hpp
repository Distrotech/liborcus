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
class xml_stream_parser
{
public:
    class parse_error : public ::std::exception
    {
    public:
        parse_error(const ::std::string& msg);
        virtual ~parse_error() throw();
        virtual const char* what() const throw();
    private:
        ::std::string m_msg;
    };

    xml_stream_parser(
        const config& opt,
        xmlns_repository& ns_repo, const tokens& tokens, const char* content, size_t size);
    ~xml_stream_parser();

    void parse();

    void set_handler(xml_stream_handler* handler);
    xml_stream_handler* get_handler() const;

private:
    xml_stream_parser(); // disabled

    const config& m_config;
    xmlns_context m_ns_cxt;
    const tokens& m_tokens;
    xml_stream_handler* mp_handler;
    const char* m_content;
    size_t m_size;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
