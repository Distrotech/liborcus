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
    virtual ~json_value() {}
};

struct json_value_string : public json_value
{
    std::string value_string;

    json_value_string() : json_value(json_value_type::string) {}
    json_value_string(const std::string& s) : json_value(json_value_type::string), value_string(s) {}
    json_value_string(const char* p, size_t n) : json_value(json_value_type::string), value_string(p, n) {}
    virtual ~json_value_string() {}
};

struct json_value_number : public json_value
{
    double value_number;

    json_value_number() : json_value(json_value_type::number) {}
    json_value_number(double num) : json_value(json_value_type::number), value_number(num) {}
    virtual ~json_value_number() {}
};

struct json_value_array : public json_value
{
    std::vector<std::unique_ptr<json_value>> value_array;

    json_value_array() : json_value(json_value_type::array) {}
    virtual ~json_value_array() {}
};

struct json_value_object : public json_value
{
    std::unordered_map<std::string, std::unique_ptr<json_value>> value_object;

    json_value_object() : json_value(json_value_type::object) {}
    virtual ~json_value_object() {}
};

void dump_repeat(std::ostringstream& os, const char* s, int repeat)
{
    for (int i = 0; i < repeat; ++i)
        os << s;
}

void dump_value(std::ostringstream& os, const json_value* v, int level, const std::string* key = nullptr)
{
    static const char* tab = "    ";
    static const char quote = '"';
    dump_repeat(os, tab, level);

    if (key)
        os << quote << *key << quote << ": ";

    switch (v->type)
    {
        case json_value_type::array:
        {
            auto& vals = static_cast<const json_value_array*>(v)->value_array;
            os << "[" << std::endl;
            size_t n = vals.size();
            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it)
            {
                dump_value(os, it->get(), level+1);
                size_t pos = std::distance(vals.begin(), it);
                if (pos < (n-1))
                    os << ",";
                os << std::endl;
            }

            dump_repeat(os, tab, level);
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
            os << static_cast<const json_value_number*>(v)->value_number;
        break;
        case json_value_type::object:
        {
            auto& vals = static_cast<const json_value_object*>(v)->value_object;
            os << "{" << std::endl;
            size_t n = vals.size();
            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it)
            {
                auto& key = it->first;
                auto& val = it->second;
                dump_value(os, val.get(), level+1, &key);
                size_t pos = std::distance(vals.begin(), it);
                if (pos < (n-1))
                    os << ",";
                os << std::endl;
            }

            dump_repeat(os, tab, level);
            os << "}" << std::endl;
        }
        break;
        case json_value_type::string:
            os << static_cast<const json_value_string*>(v)->value_string;
        break;
        case json_value_type::unset:
        default:
            ;
    }
}

void dump(const json_value* root)
{
    if (root->type == json_value_type::unset)
        return;

    std::ostringstream os;
    dump_value(os, root, 0);
    std::cout << os.str() << std::endl;
}

struct parser_stack
{
    std::string key;
    json_value* node;

    parser_stack(json_value* _node) : node(_node) {}
};

class parser_handler
{
    std::unique_ptr<json_value> m_root;
    std::vector<parser_stack> m_stack;
    std::string m_cur_object_key;

    json_value* push_value(std::unique_ptr<json_value>&& value)
    {
        assert(!m_stack.empty());
        parser_stack& cur = m_stack.back();

        switch (cur.node->type)
        {
            case json_value_type::array:
            {
                json_value_array* jva = static_cast<json_value_array*>(cur.node);
                jva->value_array.push_back(std::move(value));
                return jva->value_array.back().get();
            }
            break;
            case json_value_type::object:
            {
                const std::string& key = cur.key;
                json_value_object* jvo = static_cast<json_value_object*>(cur.node);
                auto r = jvo->value_object.insert(
                    std::make_pair(key, std::move(value)));

                return r.first->second.get();
            }
            break;
            default:
            {
                std::ostringstream os;
                os << BOOST_CURRENT_FUNCTION << ": unstackable JSON value type.";
                throw json::parse_error(os.str());
            }
        }

        return nullptr;
    }

public:
    parser_handler() {}

    void begin_parse()
    {
        m_root.reset();
    }

    void end_parse()
    {
    }

    void begin_array()
    {
        if (m_root)
        {
            json_value* jv = push_value(make_unique<json_value_array>());
            assert(jv && jv->type == json_value_type::array);
            m_stack.push_back(parser_stack(jv));
        }
        else
        {
            m_root = make_unique<json_value_array>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void end_array()
    {
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void begin_object()
    {
        if (m_root)
        {
            json_value* jv = push_value(make_unique<json_value_object>());
            assert(jv && jv->type == json_value_type::object);
            m_stack.push_back(parser_stack(jv));
        }
        else
        {
            m_root = make_unique<json_value_object>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void object_key(const char* p, size_t len)
    {
        parser_stack& cur = m_stack.back();
        cur.key = std::string(p, len);
    }

    void end_object()
    {
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void boolean_true()
    {
        push_value(make_unique<json_value>(json_value_type::boolean_true));
    }

    void boolean_false()
    {
        push_value(make_unique<json_value>(json_value_type::boolean_false));
    }

    void null()
    {
        push_value(make_unique<json_value>(json_value_type::null));
    }

    void string(const char* p, size_t len)
    {
        push_value(make_unique<json_value_string>(p, len));
    }

    void number(double val)
    {
        push_value(make_unique<json_value_number>(val));
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
        dump(mp_impl->m_root.get());
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
