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

namespace yaml { namespace detail {

struct yaml_value
{
    node_t type;
    yaml_value* parent;

    yaml_value() : type(node_t::unset), parent(nullptr) {}
    yaml_value(node_t _type) : type(_type), parent(nullptr) {}
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
            case node_t::unset:
                os << "unset";
            break;
            case node_t::string:
                os << "string";
            break;
            case node_t::number:
                os << "number";
            break;
            case node_t::map:
                os << "map";
            break;
            case node_t::sequence:
                os << "sequence";
            break;
            case node_t::boolean_true:
                os << "true";
            break;
            case node_t::boolean_false:
                os << "false";
            break;
            case node_t::null:
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

}}

namespace {

using node_t = yaml::detail::node_t;
using yaml_value = yaml::detail::yaml_value;

struct yaml_value_string : public yaml_value
{
    std::string value_string;

    yaml_value_string() : yaml_value(node_t::string) {}
    yaml_value_string(const std::string& s) : yaml_value(node_t::string), value_string(s) {}
    yaml_value_string(const char* p, size_t n) : yaml_value(node_t::string), value_string(p, n) {}
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

    yaml_value_number() : yaml_value(node_t::number) {}
    yaml_value_number(double num) : yaml_value(node_t::number), value_number(num) {}
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

    yaml_value_sequence() : yaml_value(node_t::sequence) {}
    virtual ~yaml_value_sequence() {}
};

struct yaml_value_map : public yaml_value
{
    std::vector<std::unique_ptr<yaml_value>> key_order;  // owns the key instances.
    std::unordered_map<const yaml_value*, std::unique_ptr<yaml_value>> value_map;

    yaml_value_map() : yaml_value(node_t::map) {}
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

    bool m_in_document;

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
            case node_t::sequence:
            {
                yaml_value_sequence* yvs = static_cast<yaml_value_sequence*>(cur.node);
                value->parent = yvs;
                yvs->value_sequence.push_back(std::move(value));
                return yvs->value_sequence.back().get();
            }
            break;
            case node_t::map:
            {
                yaml_value_map* yvm = static_cast<yaml_value_map*>(cur.node);
                value->parent = yvm;

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
    handler() : m_in_document(false) {}

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
        assert(!m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_document): " << std::endl;
        m_in_document = true;
        m_root.reset();
    }

    void end_document()
    {
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:end_document): " << std::endl;
        assert(m_stack.empty());
        m_in_document = false;
        m_docs.push_back(std::move(m_root));
    }

    void begin_sequence()
    {
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_sequence): " << std::endl;

        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_sequence>());
            assert(yv && yv->type == node_t::sequence);
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
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:begin_map): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_map>());
            assert(yv && yv->type == node_t::map);
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
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:string): s='" << pstring(p, n) << "'" << std::endl;

        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_string>(p, n));
            assert(yv && yv->type == node_t::string);
        }
        else
            m_root = make_unique<yaml_value_string>(p, n);
    }

    void number(double val)
    {
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:number): v=" << val << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value_number>(val));
            assert(yv && yv->type == node_t::number);
        }
        else
            m_root = make_unique<yaml_value_number>(val);
    }

    void boolean_true()
    {
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_true): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(node_t::boolean_true));
            assert(yv && yv->type == node_t::boolean_true);
        }
        else
            m_root = make_unique<yaml_value>(node_t::boolean_true);
    }

    void boolean_false()
    {
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:boolean_false): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(node_t::boolean_false));
            assert(yv && yv->type == node_t::boolean_false);
        }
        else
            m_root = make_unique<yaml_value>(node_t::boolean_false);
    }

    void null()
    {
        assert(m_in_document);
        std::cout << __FILE__ << "#" << __LINE__ << " (handler:null): " << std::endl;
        if (m_root)
        {
            yaml_value* yv = push_value(make_unique<yaml_value>(node_t::null));
            assert(yv && yv->type == node_t::null);
        }
        else
            m_root = make_unique<yaml_value>(node_t::null);
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

struct node::impl
{
    const yaml_value* m_node;

    impl(const yaml_value* yv) : m_node(yv) {}
};

node::node(const yaml_value* yv) : mp_impl(make_unique<impl>(yv)) {}
node::node(const node& other) : mp_impl(make_unique<impl>(other.mp_impl->m_node)) {}
node::node(node&& rhs) : mp_impl(std::move(rhs.mp_impl)) {}
node::~node() {}

node& node::operator=(const node& other)
{
    if (this == &other)
        return *this;

    node tmp(other);
    mp_impl.swap(tmp.mp_impl);
    return *this;
}

node_t node::type() const
{
    return mp_impl->m_node->type;
}

size_t node::child_count() const
{
    switch (mp_impl->m_node->type)
    {
        case node_t::map:
            return static_cast<const yaml_value_map*>(mp_impl->m_node)->value_map.size();
        case node_t::sequence:
            return static_cast<const yaml_value_sequence*>(mp_impl->m_node)->value_sequence.size();
        case node_t::string:
        case node_t::number:
        case node_t::boolean_true:
        case node_t::boolean_false:
        case node_t::null:
        case node_t::unset:
        default:
            ;
    }
    return 0;
}

node node::key(size_t index) const
{
    if (mp_impl->m_node->type != node_t::map)
        throw yaml_document_error("node::key: this node is not of map type.");

    const yaml_value_map* yvm = static_cast<const yaml_value_map*>(mp_impl->m_node);
    if (index >= yvm->key_order.size())
        throw std::out_of_range("node::key: index is out-of-range.");

    return node(yvm->key_order[index].get());
}

node node::child(size_t index) const
{
    switch (mp_impl->m_node->type)
    {
        case node_t::map:
        {
            const yaml_value_map* yvm = static_cast<const yaml_value_map*>(mp_impl->m_node);
            if (index >= yvm->key_order.size())
                throw std::out_of_range("node::child: index is out-of-range");

            const yaml_value* key = yvm->key_order[index].get();
            auto it = yvm->value_map.find(key);
            assert(it != yvm->value_map.end());
            return node(it->second.get());
        }
        break;
        case node_t::sequence:
        {
            const yaml_value_sequence* yvs = static_cast<const yaml_value_sequence*>(mp_impl->m_node);
            if (index >= yvs->value_sequence.size())
                throw std::out_of_range("node::child: index is out-of-range");

            return node(yvs->value_sequence[index].get());
        }
        break;
        case node_t::string:
        case node_t::number:
        case node_t::boolean_true:
        case node_t::boolean_false:
        case node_t::null:
        case node_t::unset:
        default:
            throw yaml_document_error("node::child: this node cannot have child nodes.");
    }
}

node node::parent() const
{
    if (!mp_impl->m_node->parent)
        throw yaml_document_error("node::parent: this node has no parent.");

    return node(mp_impl->m_node->parent);
}

pstring node::string_value() const
{
    if (mp_impl->m_node->type != node_t::string)
        throw yaml_document_error("node::key: current node is not of string type.");

    const yaml_value_string* yvs = static_cast<const yaml_value_string*>(mp_impl->m_node);
    const std::string& str = yvs->value_string;
    return pstring(str.data(), str.size());
}

double node::numeric_value() const
{
    if (mp_impl->m_node->type != node_t::number)
        throw yaml_document_error("node::key: current node is not of numeric type.");

    const yaml_value_number* yvn = static_cast<const yaml_value_number*>(mp_impl->m_node);
    return yvn->value_number;
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

size_t yaml_document_tree::get_document_count() const
{
    return mp_impl->m_docs.size();
}

yaml_document_tree::node yaml_document_tree::get_document_root(size_t index) const
{
    return node(mp_impl->m_docs[index].get());
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
