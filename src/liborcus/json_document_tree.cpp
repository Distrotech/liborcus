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
#include <sstream>

#include <boost/current_function.hpp>

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
    json_value_string(const char* p, size_t n) : json_value(json_value_type::string), value_string(p, n) {}
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

void dump_value(std::ostringstream& os, const json_value& v, int level)
{
    static const char* tab = "    ";
    for (int i = 0; i < level; ++i)
        os << tab;

    switch (v.type)
    {
        case json_value_type::array:
        {
            const std::vector<json_value>& vals = static_cast<const json_value_array&>(v).value_array;
            os << "[" << std::endl;
            size_t n = vals.size();
            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it)
            {
                dump_value(os, *it, level+1);
                size_t pos = std::distance(vals.begin(), it);
                if (pos < (n-1))
                    os << ",";
                os << std::endl;
            }
            os << "]" << std::endl;
        }
        break;
        case json_value_type::boolean_false:
            os << "false";
        break;
        case json_value_type::boolean_true:
            os << "true";
        break;
        case json_value_type::null:
            os << "null";
        break;
        case json_value_type::number:
            os << static_cast<const json_value_number&>(v).value_number;
        break;
        case json_value_type::object:
        {
            os << "{" << std::endl;
            os << "}" << std::endl;
        }
        break;
        case json_value_type::string:
            os << static_cast<const json_value_string&>(v).value_string;
        break;
        case json_value_type::unset:
        default:
            ;
    }
}

void dump(const json_value& root)
{
    if (root.type == json_value_type::unset)
        return;

    std::ostringstream os;
    dump_value(os, root, 0);
    std::cout << os.str() << std::endl;
}

class parser_handler
{
    std::unique_ptr<json_value> m_root;
    std::vector<json_value*> m_stack;


    void push_value(const json_value& value)
    {
        assert(!m_stack.empty());
        json_value* cur = m_stack.back();

        switch (cur->type)
        {
            case json_value_type::array:
            {
                json_value_array* jva = static_cast<json_value_array*>(cur);
                jva->value_array.push_back(value);
            }
            break;
            case json_value_type::object:
            break;
            default:
            {
                std::ostringstream os;
                os << BOOST_CURRENT_FUNCTION << ": unstackable JSON value type.";
                throw json::parse_error(os.str());
            }
        }
    }

public:
    parser_handler() {}

    void begin_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:begin_parse): " << std::endl;
        m_root.reset();
    }

    void end_parse()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:end_parse): " << std::endl;
    }

    void begin_array()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:begin_array): " << std::endl;
        if (!m_root)
        {
            m_root = make_unique<json_value_array>();
            m_stack.push_back(m_root.get());
        }
    }

    void end_array()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:end_array): " << std::endl;
        assert(!m_stack.empty());
        m_stack.pop_back();
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
        push_value(json_value(json_value_type::boolean_true));
    }

    void boolean_false()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:boolean_false): " << std::endl;
        push_value(json_value(json_value_type::boolean_false));
    }

    void null()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:null): " << std::endl;
        push_value(json_value(json_value_type::null));
    }

    void string(const char* p, size_t len)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:string): '" << pstring(p, len) << "'" << std::endl;
        push_value(json_value_string(p, len));
    }

    void number(double val)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (parser_handler:number): " << val << std::endl;
        push_value(json_value_number(val));
    }

    void swap(std::unique_ptr<json_value>& other)
    {
        other.swap(m_root);
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
    parser_handler hdl;
    json_parser<parser_handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
    hdl.swap(mp_impl->m_root);
    if (mp_impl->m_root)
        dump(*mp_impl->m_root);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
