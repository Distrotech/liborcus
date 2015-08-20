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
#include <vector>
#include <memory>
#include <unordered_map>

namespace orcus {

namespace {

enum class yaml_value_type
{
    unset,
    string,
    number,
    map,
    sequence,
    boolean_true,
    boolean_false,
    null
};

struct yaml_value
{
    yaml_value_type type;

    yaml_value() : type(yaml_value_type::unset) {}
    yaml_value(yaml_value_type _type) : type(_type) {}
    virtual ~yaml_value() {}
};

struct yaml_value_string : public yaml_value
{
    std::string value_string;

    yaml_value_string() : yaml_value(yaml_value_type::string) {}
    yaml_value_string(const std::string& s) : yaml_value(yaml_value_type::string), value_string(s) {}
    yaml_value_string(const char* p, size_t n) : yaml_value(yaml_value_type::string), value_string(p, n) {}
    virtual ~yaml_value_string() {}
};

struct yaml_value_number : public yaml_value
{
    double value_number;

    yaml_value_number() : yaml_value(yaml_value_type::number) {}
    yaml_value_number(double num) : yaml_value(yaml_value_type::number), value_number(num) {}
    virtual ~yaml_value_number() {}
};

struct yaml_value_sequence : public yaml_value
{
    std::vector<std::unique_ptr<yaml_value>> value_array;

    yaml_value_sequence() : yaml_value(yaml_value_type::sequence) {}
    virtual ~yaml_value_sequence() {}
};

struct yaml_value_map : public yaml_value
{
    std::vector<std::string> key_order;
    std::unordered_map<std::string, std::unique_ptr<yaml_value>> value_object;

    bool has_ref;

    yaml_value_map() : yaml_value(yaml_value_type::map), has_ref(false) {}
    virtual ~yaml_value_map() {}

    void swap(yaml_value_map& src)
    {
        key_order.swap(src.key_order);
        value_object.swap(src.value_object);
    }
};

}

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
