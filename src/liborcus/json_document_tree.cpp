/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/json_document_tree.hpp"
#include "orcus/json_parser.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace orcus {

namespace {

enum class json_value_type
{
    unset,
    string,
    number,
    object,
    array,
    boolean_true,
    boolean_false,
    null
};

struct json_value
{
    json_value_type type;

    json_value() : type(json_value_type::unset) {}
    json_value(json_value_type _type) : type(_type) {}
    json_value(const json_value& r) : type(r.type) {}
    virtual ~json_value() {}
};

struct json_value_string : public json_value
{
    std::string value_string;

    json_value_string() : json_value(json_value_type::string) {}
    json_value_string(const std::string& s) : json_value(json_value_type::string), value_string(s) {}
    json_value_string(const json_value_string& r) : json_value(r), value_string(r.value_string) {}
    virtual ~json_value_string() {}
};

struct json_value_number : public json_value
{
    double value_number;

    json_value_number() : json_value(json_value_type::number) {}
    json_value_number(double num) : json_value(json_value_type::number), value_number(num) {}
    json_value_number(const json_value_number& r) : json_value(r), value_number(r.value_number) {}
    virtual ~json_value_number() {}
};

struct json_value_array : public json_value
{
    std::vector<json_value> value_array;

    json_value_array() : json_value(json_value_type::array) {}
    json_value_array(const json_value_array& r) : json_value(r), value_array(r.value_array) {}
    virtual ~json_value_array() {}
};

struct json_value_object : public json_value
{
    std::unordered_map<std::string, json_value> value_object;

    json_value_object() : json_value(json_value_type::object) {}
    json_value_object(const json_value_object& r) : value_object(r.value_object) {}
    virtual ~json_value_object() {}
};

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

    void object_key(const char* p, size_t len)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:object_key): '" << pstring(p, len) << "'" << std::endl;
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

struct json_document_tree::impl
{
    std::unique_ptr<json_value> m_root;
};

json_document_tree::json_document_tree() : mp_impl(make_unique<impl>()) {}
json_document_tree::~json_document_tree() {}

void json_document_tree::load(const std::string& strm)
{
    parser_handler hdl(*this);
    json_parser<parser_handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
