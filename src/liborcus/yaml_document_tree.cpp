/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/yaml_parser.hpp"
#include "orcus/pstring.hpp"

#include <iostream>

namespace orcus {

class handler
{
public:
    void begin_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_parse): " << std::endl;
    }

    void end_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_parse): " << std::endl;
    }

    void begin_document()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_document): " << std::endl;
    }

    void end_document()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_document): " << std::endl;
    }

    void begin_sequence()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_sequence): " << std::endl;
    }

    void end_sequence()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_sequence): " << std::endl;
    }

    void begin_map()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map): " << std::endl;
    }

    void begin_map_key()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map_key): " << std::endl;
    }

    void end_map_key()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_map_key): " << std::endl;
    }

    void end_map()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_map): " << std::endl;
    }

    void string(const char* p, size_t n)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:string): s='" << pstring(p, n) << "'" << std::endl;
    }

    void number(double val)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:number): v=" << val << std::endl;
    }

    void boolean_true()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_true): " << std::endl;
    }

    void boolean_false()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_false): " << std::endl;
    }

    void null()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:null): " << std::endl;
    }
};

yaml_document_tree::yaml_document_tree() {}
yaml_document_tree::~yaml_document_tree() {}

void yaml_document_tree::load(const std::string& strm)
{
    handler hdl;
    yaml_parser<handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
