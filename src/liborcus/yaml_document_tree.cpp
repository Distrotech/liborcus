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

yaml_document_error::yaml_document_error(const std::string& msg) :
    general_error("yaml_document_error", msg) {}

yaml_document_error::~yaml_document_error() throw() {}

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
        os << "type: ";
        switch (type)
        {
            case yaml_value_type::unset:
                os << "unset";
            break;
            case yaml_value_type::string:
                os << "string";
            break;
            case yaml_value_type::number:
                os << "number";
            break;
            case yaml_value_type::map:
                os << "map";
            break;
            case yaml_value_type::sequence:
                os << "sequence";
            break;
            case yaml_value_type::boolean_true:
                os << "true";
            break;
            case yaml_value_type::boolean_false:
                os << "false";
            break;
            case yaml_value_type::null:
                os << "null";
            break;
        }
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

typedef std::unique_ptr<yaml_value> document_root_type;

class handler
{
    std::vector<document_root_type> m_docs;

    std::vector<parser_stack> m_stack;
    std::vector<parser_stack> m_key_stack;

    document_root_type m_root;
    document_root_type m_key_root;

    void print_stack()
    {
        std::ostringstream os;
        os << '(';
        for (auto i = m_stack.begin(), ie = m_stack.end(); i != ie; ++i)
            os << i->node->print() << ',';
        os << ')';
        std::cout << os.str() << std::endl;
    }

    yaml_value* push_value(std::unique_ptr<yaml_value>&& value)
    {
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
                yaml_value_map* yvm = static_cast<yaml_value_map*>(cur.node);

                yvm->key_order.push_back(std::move(cur.key));

                auto r = yvm->value_map.insert(
                    std::make_pair(
                        yvm->key_order.back().get(), std::move(value)));

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
        m_docs.push_back(std::move(m_root));
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
        }
        else
            m_root = make_unique<yaml_value_string>(p, n);
    }

    void number(double val)
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:number): v=" << val << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_number>(val));
            assert(yv && yv->type == yaml_value_type::number);
        }
        else
            m_root = make_unique<yaml_value_number>(val);
    }

    void boolean_true()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_true): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(yaml_value_type::boolean_true));
            assert(yv && yv->type == yaml_value_type::boolean_true);
        }
        else
            m_root = make_unique<yaml_value>(yaml_value_type::boolean_true);
    }

    void boolean_false()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_false): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(yaml_value_type::boolean_false));
            assert(yv && yv->type == yaml_value_type::boolean_false);
        }
        else
            m_root = make_unique<yaml_value>(yaml_value_type::boolean_false);
    }

    void null()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:null): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(yaml_value_type::null));
            assert(yv && yv->type == yaml_value_type::null);
        }
        else
            m_root = make_unique<yaml_value>(yaml_value_type::null);
    }

    void swap(std::vector<document_root_type>& docs)
    {
        m_docs.swap(docs);
    }
};

}

struct yaml_document_tree::impl
{
    std::vector<document_root_type> m_docs;
};

namespace yaml { namespace detail {

node_t to_public_node_type(yaml_value_type type)
{
    switch (type)
    {
        case yaml_value_type::string:
            return node_t::string;
        case yaml_value_type::number:
            return node_t::number;
        case yaml_value_type::map:
            return node_t::map;
        case yaml_value_type::sequence:
            return node_t::sequence;
        case yaml_value_type::boolean_true:
            return node_t::boolean_true;
        case yaml_value_type::boolean_false:
            return node_t::boolean_false;
        case yaml_value_type::null:
            return node_t::null;
        case yaml_value_type::unset:
            return node_t::unset;
        default:
            throw yaml_document_error("to_public_node_type: unknown node type.");
    }
}

struct tree_walker::impl
{
    const std::vector<document_root_type>& m_docs;

    const yaml_value* m_node;
    node_t m_node_type;

    impl() = delete;
    impl(const impl&) = delete;
    impl(impl&& rhs) = default;

    impl(std::vector<document_root_type>& docs) :
        m_docs(docs),
        m_node(nullptr),
        m_node_type(node_t::document_list) {}

    void set_node(const yaml_value* node)
    {
        m_node = node;
        m_node_type = to_public_node_type(node->type);
    }
};

tree_walker::tree_walker(const yaml_document_tree& parent) :
    mp_impl(make_unique<impl>(parent.mp_impl->m_docs)) {}

tree_walker::tree_walker(tree_walker&& rhs) :
    mp_impl(std::move(rhs.mp_impl)) {}

tree_walker::~tree_walker() {}

node_t tree_walker::type() const
{
    return mp_impl->m_node_type;
}

size_t tree_walker::child_count() const
{
    switch (mp_impl->m_node_type)
    {
        case node_t::document_list:
            return mp_impl->m_docs.size();
        case node_t::map:
        {
            const yaml_value_map* node = static_cast<const yaml_value_map*>(mp_impl->m_node);
            return node->value_map.size();
        }
        default:
            ;
    }

    return 0;
}

void tree_walker::first_child()
{
    switch (mp_impl->m_node_type)
    {
        case node_t::document_list:
            if (mp_impl->m_docs.empty())
                throw yaml_document_error("first_child: document list is empty.");
            mp_impl->set_node(mp_impl->m_docs[0].get());
        break;
        default:
            ;
    }
}

map_keys tree_walker::keys() const
{
    if (mp_impl->m_node_type != node_t::map)
        throw yaml_document_error("keys: current node type is not map.");

    return map_keys(*this);
}

struct map_keys::impl
{
    const yaml_value_map* m_root;

    const yaml_value* m_node;

    impl(const yaml_value_map* node) : m_root(node), m_node(nullptr) {}
};

map_keys::map_keys(const tree_walker& parent) :
    mp_impl(
        make_unique<impl>(
            static_cast<const yaml_value_map*>(parent.mp_impl->m_node))) {}

map_keys::map_keys(map_keys&& rhs) :
    mp_impl(std::move(rhs.mp_impl)) {}

map_keys::~map_keys() {}

node_t map_keys::type() const
{
    return to_public_node_type(
        mp_impl->m_node ? mp_impl->m_node->type : mp_impl->m_root->type);
}

size_t map_keys::child_count() const
{
    if (mp_impl->m_node)
    {
        return 0;
    }

    return mp_impl->m_root->value_map.size();
}

}}

yaml_document_tree::yaml_document_tree() : mp_impl(make_unique<impl>()) {}
yaml_document_tree::~yaml_document_tree() {}

void yaml_document_tree::load(const std::string& strm)
{
    handler hdl;
    yaml_parser<handler> parser(strm.data(), strm.size(), hdl);
    parser.parse();
    hdl.swap(mp_impl->m_docs);
}

yaml_document_tree::walker yaml_document_tree::get_walker() const
{
    return walker(*this);
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
