/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_document_tree.hpp"
#include "orcus/json_parser.hpp"
#include "orcus/pstring.hpp"

#include <iostream>

namespace orcus {

namespace {

class parser_handler
{
    json_document_tree& m_doc;
public:
    parser_handler(json_document_tree& doc) : m_doc(doc) {}

    void begin_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:begin_parse): " << std::endl;
    }

    void end_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:end_parse): " << std::endl;
    }

    void begin_array()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:begin_array): " << std::endl;
    }

    void end_array()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:end_array): " << std::endl;
    }

    void begin_object()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:begin_object): " << std::endl;
    }

    void end_object()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:end_object): " << std::endl;
    }

    void boolean_true()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:boolean_true): " << std::endl;
    }

    void boolean_false()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:boolean_false): " << std::endl;
    }

    void null()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:null): " << std::endl;
    }

    void string(const char* p, size_t len)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:string): '" << pstring(p, len) << "'" << std::endl;
    }

    void number(double val)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:number): " << val << std::endl;
    }
};

}

json_document_tree::json_document_tree() {}
json_document_tree::~json_document_tree() {}

void json_document_tree::load(const std::string& strm)
{
    parser_handler hdl(*this);
    json_parser<parser_handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
