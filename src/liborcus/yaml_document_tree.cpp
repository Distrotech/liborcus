/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/yaml_document_tree.hpp"
#include "orcus/yaml_parser.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <unordered_map>

#include <boost/current_function.hpp>

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

    virtual size_t get_hash() const
    {
        return reinterpret_cast<size_t>(this);
    }

    virtual std::string print() const
    {
        std::ostringstream os;
        os << "type: " << static_cast<int>(type);
        return os.str();
    }

    struct hash
    {
        size_t operator() (const yaml_value& v) const
        {
            return v.get_hash();
        }
    };
};

struct yaml_value_string : public yaml_value
{
    std::string value_string;

    yaml_value_string() : yaml_value(yaml_value_type::string) {}
    yaml_value_string(const std::string& s) : yaml_value(yaml_value_type::string), value_string(s) {}
    yaml_value_string(const char* p, size_t n) : yaml_value(yaml_value_type::string), value_string(p, n) {}
    virtual ~yaml_value_string() {}

    virtual std::string print() const
    {
        std::ostringstream os;
        os << "type: string, value: " << value_string;
        return os.str();
    }
};

struct yaml_value_number : public yaml_value
{
    double value_number;

    yaml_value_number() : yaml_value(yaml_value_type::number) {}
    yaml_value_number(double num) : yaml_value(yaml_value_type::number), value_number(num) {}
    virtual ~yaml_value_number() {}

    virtual std::string print() const
    {
        std::ostringstream os;
        os << "type: number, value: " << value_number;
        return os.str();
    }
};

struct yaml_value_sequence : public yaml_value
{
    std::vector<std::unique_ptr<yaml_value>> value_sequence;

    yaml_value_sequence() : yaml_value(yaml_value_type::sequence) {}
    virtual ~yaml_value_sequence() {}
};

struct yaml_value_map : public yaml_value
{
    std::vector<std::unique_ptr<yaml_value>> key_order;  // owns the key instances.
    std::unordered_map<const yaml_value*, std::unique_ptr<yaml_value>> value_map;

    yaml_value_map() : yaml_value(yaml_value_type::map) {}
    virtual ~yaml_value_map() {}
};

struct parser_stack
{
    std::unique_ptr<yaml_value> key;
    yaml_value* node;

    parser_stack(yaml_value* _node) : node(_node) {}
};

}

class handler
{
    typedef std::unique_ptr<yaml_value> root_type;
    std::vector<root_type> m_docs;

    std::vector<parser_stack> m_stack;
    std::vector<parser_stack> m_key_stack;

    root_type m_root;
    root_type m_key_root;

    yaml_value* push_value(std::unique_ptr<yaml_value>&& value)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:push_value): value = " << value.get() << std::endl;
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:push_value): value type = " << (int)value->type << std::endl;
        assert(!m_stack.empty());
        parser_stack& cur = m_stack.back();

        switch (cur.node->type)
        {
            case yaml_value_type::sequence:
            {
                yaml_value_sequence* yvs = static_cast<yaml_value_sequence*>(cur.node);
                yvs->value_sequence.push_back(std::move(value));
                return yvs->value_sequence.back().get();
            }
            break;
            case yaml_value_type::map:
            {
                std::cout << __FILE__ << "#" << __LINE__ << " (handler:push_value): map" << std::endl;
                yaml_value_map* yvm = static_cast<yaml_value_map*>(cur.node);

                yvm->key_order.push_back(std::move(cur.key));

                auto r = yvm->value_map.insert(
                    std::make_pair(
                        yvm->key_order.back().get(), std::move(value)));

                std::cout << __FILE__ << "#" << __LINE__ << " (handler:push_value): " << r.first->second.get() << std::endl;
                return r.first->second.get();
            }
            break;
            default:
            {
                std::ostringstream os;
                os << BOOST_CURRENT_FUNCTION << ": unstackable YAML value type (" << cur.node->print() << ").";
                throw yaml::parse_error(os.str());
            }
        }

        return nullptr;
    }

public:
    handler() {}

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
        m_root.reset();
    }

    void end_document()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_document): " << std::endl;
    }

    void begin_sequence()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_sequence): " << std::endl;

        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_sequence>());
            assert(yv && yv->type == yaml_value_type::sequence);
            m_stack.push_back(parser_stack(yv));
        }
        else
        {
            m_root = make_unique<yaml_value_sequence>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void end_sequence()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_sequence): " << std::endl;
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void begin_map()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_map>());
            std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map): yv = " << yv << ", type = " << (int)yv->type << std::endl;
            assert(yv && yv->type == yaml_value_type::map);
            m_stack.push_back(parser_stack(yv));
        }
        else
        {
            m_root = make_unique<yaml_value_map>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void begin_map_key()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map_key): " << std::endl;
        assert(!m_key_root);
        assert(m_key_stack.empty());
        m_key_root.swap(m_root);
        m_key_stack.swap(m_stack);
    }

    void end_map_key()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_map_key): " << std::endl;
        m_key_root.swap(m_root);
        m_key_stack.swap(m_stack);

        assert(!m_stack.empty());
        parser_stack& cur = m_stack.back();
        cur.key.swap(m_key_root);
        m_key_stack.clear();
        m_key_root.reset();
    }

    void end_map()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_map): " << std::endl;
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void string(const char* p, size_t n)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:string): s='" << pstring(p, n) << "'" << std::endl;

        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_string>(p, n));
            assert(yv && yv->type == yaml_value_type::string);
            m_stack.push_back(parser_stack(yv));
        }
        else
        {
            m_root = make_unique<yaml_value_string>(p, n);
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void number(double val)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:number): v=" << val << std::endl;
        push_value(make_unique<yaml_value_number>(val));
    }

    void boolean_true()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_true): " << std::endl;
        push_value(make_unique<yaml_value>(yaml_value_type::boolean_true));
    }

    void boolean_false()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_false): " << std::endl;
        push_value(make_unique<yaml_value>(yaml_value_type::boolean_false));
    }

    void null()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:null): " << std::endl;
        push_value(make_unique<yaml_value>(yaml_value_type::null));
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
