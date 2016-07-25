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

#include "json_util.hpp"

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iostream>

#include <boost/current_function.hpp>

#define ORCUS_DEBUG_YAML_TREE 0

#if ORCUS_DEBUG_YAML_TREE
#include <iostream>
#endif

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
    parser_stack(const parser_stack&) = delete;
    parser_stack(parser_stack&& r) : key(std::move(r.key)), node(r.node) {}
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

#if ORCUS_DEBUG_YAML_TREE
    void print_stack()
    {
        std::ostringstream os;
        os << '(';
        for (auto i = m_stack.begin(), ie = m_stack.end(); i != ie; ++i)
            os << i->node->print() << ',';
        os << ')';
        std::cout << os.str() << std::endl;
    }
#endif

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
    }

    void end_parse()
    {
    }

    void begin_document()
    {
        assert(!m_in_document);
        m_in_document = true;
        m_root.reset();
    }

    void end_document()
    {
        assert(m_stack.empty());
        m_in_document = false;
        m_docs.push_back(std::move(m_root));
    }

    void begin_sequence()
    {
        assert(m_in_document);

        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value_sequence>());
            assert(yv && yv->type == node_t::sequence);
            m_stack.push_back(parser_stack(yv));
        }
        else
        {
            m_root = orcus::make_unique<yaml_value_sequence>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void end_sequence()
    {
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void begin_map()
    {
        assert(m_in_document);
        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value_map>());
            assert(yv && yv->type == node_t::map);
            m_stack.push_back(parser_stack(yv));
        }
        else
        {
            m_root = orcus::make_unique<yaml_value_map>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void begin_map_key()
    {
        assert(!m_key_root);
        assert(m_key_stack.empty());
        m_key_root.swap(m_root);
        m_key_stack.swap(m_stack);
    }

    void end_map_key()
    {
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
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void string(const char* p, size_t n)
    {
        assert(m_in_document);

        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value_string>(p, n));
            assert(yv && yv->type == node_t::string);
        }
        else
            m_root = orcus::make_unique<yaml_value_string>(p, n);
    }

    void number(double val)
    {
        assert(m_in_document);
        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value_number>(val));
            assert(yv && yv->type == node_t::number);
        }
        else
            m_root = orcus::make_unique<yaml_value_number>(val);
    }

    void boolean_true()
    {
        assert(m_in_document);
        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value>(node_t::boolean_true));
            assert(yv && yv->type == node_t::boolean_true);
        }
        else
            m_root = orcus::make_unique<yaml_value>(node_t::boolean_true);
    }

    void boolean_false()
    {
        assert(m_in_document);
        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value>(node_t::boolean_false));
            assert(yv && yv->type == node_t::boolean_false);
        }
        else
            m_root = orcus::make_unique<yaml_value>(node_t::boolean_false);
    }

    void null()
    {
        assert(m_in_document);
        if (m_root)
        {
            yaml_value* yv = push_value(orcus::make_unique<yaml_value>(node_t::null));
            assert(yv && yv->type == node_t::null);
        }
        else
            m_root = orcus::make_unique<yaml_value>(node_t::null);
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

node::node(const yaml_value* yv) : mp_impl(orcus::make_unique<impl>(yv)) {}
node::node(const node& other) : mp_impl(orcus::make_unique<impl>(other.mp_impl->m_node)) {}
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

uintptr_t node::identity() const
{
    return reinterpret_cast<uintptr_t>(mp_impl->m_node);
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

std::vector<node> node::keys() const
{
    if (mp_impl->m_node->type != node_t::map)
        throw yaml_document_error("node::keys: this node is not of map type.");

    const yaml_value_map* yvm = static_cast<const yaml_value_map*>(mp_impl->m_node);
    std::vector<node> keys;
    std::for_each(yvm->key_order.begin(), yvm->key_order.end(),
        [&](const std::unique_ptr<yaml_value>& key)
        {
            keys.push_back(node(key.get()));
        }
    );

    return keys;
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

node node::child(const node& key) const
{
    if (mp_impl->m_node->type != node_t::map)
        throw yaml_document_error("node::child: this node is not of map type.");

    const yaml_value_map* yvm = static_cast<const yaml_value_map*>(mp_impl->m_node);
    auto it = yvm->value_map.find(key.mp_impl->m_node);
    if (it == yvm->value_map.end())
        throw yaml_document_error("node::child: this map does not have the specified key.");

    return node(it->second.get());
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

yaml_document_tree::yaml_document_tree() : mp_impl(orcus::make_unique<impl>()) {}
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

namespace {

const char* indent = "    ";
const char* kw_true = "true";
const char* kw_false = "false";
const char* kw_tilde = "~";
const char* kw_null = "null";

const char quote = '"';

void dump_indent(std::ostringstream& os, size_t scope)
{
    for (size_t i = 0; i < scope; ++i)
        os << indent;
}

bool needs_quoting(const std::string& s)
{
    // See if it contains certain characters...
    for (auto it = s.begin(), ite = s.end(); it != ite; ++it)
        if (is_in(*it, "#'"))
            return true;

    // See if the whole string is parsed as a number.
    const char* p = s.data();
    const char* p_end = p + s.size();
    parse_numeric(p, s.size());
    if (p == p_end)
        return true;

    return false;
}

void dump_yaml_string(std::ostringstream& os, const std::string& s)
{
    if (needs_quoting(s))
        os << quote << s << quote;
    else
        os << s;
}

void dump_yaml_map(std::ostringstream& os, const yaml_value& node, size_t scope);
void dump_yaml_sequence(std::ostringstream& os, const yaml_value& node, size_t scope);

void dump_yaml_node(std::ostringstream& os, const yaml_value& node, size_t scope)
{
    switch (node.type)
    {
        case yaml_node_t::map:
            dump_yaml_map(os, node, scope);
        break;
        case yaml_node_t::sequence:
            dump_yaml_sequence(os, node, scope);
        break;
        case yaml_node_t::boolean_true:
            dump_indent(os, scope);
            os << kw_true << std::endl;
        break;
        case yaml_node_t::boolean_false:
            dump_indent(os, scope);
            os << kw_false << std::endl;
        break;
        case yaml_node_t::null:
            dump_indent(os, scope);
            os << kw_tilde << std::endl;
        break;
        case yaml_node_t::number:
            dump_indent(os, scope);
            os << static_cast<const yaml_value_number&>(node).value_number << std::endl;
        break;
        case yaml_node_t::string:
            dump_indent(os, scope);
            dump_yaml_string(os, static_cast<const yaml_value_string&>(node).value_string);
            os << std::endl;
        break;
        case yaml_node_t::unset:
        default:
            ;
    }
}

void dump_yaml_container_item(std::ostringstream& os, const yaml_value& node, size_t scope)
{
    switch (node.type)
    {
        case yaml_node_t::map:
        case yaml_node_t::sequence:
            // End the line and dump this child container in the next scope.
            os << std::endl;
            dump_yaml_node(os, node, scope+1);
        break;
        default:
            // Dump inline.
            os << " ";
            dump_yaml_node(os, node, 0);
    }
}

void dump_yaml_map(std::ostringstream& os, const yaml_value& node, size_t scope)
{
    const yaml_value_map& yvm = static_cast<const yaml_value_map&>(node);

    std::for_each(yvm.key_order.begin(), yvm.key_order.end(),
        [&](const std::unique_ptr<yaml_value>& pkey)
        {
            const yaml_value* key = pkey.get();

            switch (key->type)
            {
                case yaml_node_t::map:
                    // TODO
                break;
                case yaml_node_t::sequence:
                    // TODO
                break;
                case yaml_node_t::boolean_true:
                    dump_indent(os, scope);
                    os << kw_true;
                break;
                case yaml_node_t::boolean_false:
                    dump_indent(os, scope);
                    os << kw_false;
                break;
                case yaml_node_t::null:
                    dump_indent(os, scope);
                    os << kw_tilde;
                break;
                case yaml_node_t::number:
                    dump_indent(os, scope);
                    os << static_cast<const yaml_value_number*>(key)->value_number;
                break;
                case yaml_node_t::string:
                    dump_indent(os, scope);
                    dump_yaml_string(os, static_cast<const yaml_value_string*>(key)->value_string);
                break;
                case yaml_node_t::unset:
                default:
                    ;
            }

            os << ":";
            auto it = yvm.value_map.find(key);
            assert(it != yvm.value_map.end());
            const yaml_value& value = *it->second;
            dump_yaml_container_item(os, value, scope);
        }
    );
}

void dump_yaml_sequence(std::ostringstream& os, const yaml_value& node, size_t scope)
{
    const yaml_value_sequence& yvs = static_cast<const yaml_value_sequence&>(node);

    std::for_each(yvs.value_sequence.begin(), yvs.value_sequence.end(),
        [&](const std::unique_ptr<yaml_value>& p)
        {
            const yaml_value& child = *p;
            dump_indent(os, scope);
            os << "-";
            dump_yaml_container_item(os, child, scope);
        }
    );
}

void dump_yaml_document(std::ostringstream& os, const yaml_value& root)
{
    os << "---" << std::endl;
    dump_yaml_node(os, root, 0);
}

void dump_json_node(std::ostringstream& os, const yaml_value& node, size_t scope, const std::string* key);

void dump_json_item(
    std::ostringstream& os, const std::string* key, const yaml_value& val,
    size_t scope, bool sep)
{
    dump_json_node(os, val, scope+1, key);
    if (sep)
        os << ",";
    os << std::endl;
}

void dump_json_node(std::ostringstream& os, const yaml_value& node, size_t scope, const std::string* key = nullptr)
{
    dump_indent(os, scope);

    if (key)
    {
        os << quote << *key << quote << ": ";
    }

    switch (node.type)
    {
        case yaml_node_t::map:
        {
            auto& key_order = static_cast<const yaml_value_map&>(node).key_order;
            auto& vals = static_cast<const yaml_value_map&>(node).value_map;
            os << "{" << std::endl;
            size_t n = vals.size();

            // Dump them based on key's original ordering.
            size_t pos = 0;
            for (auto it = key_order.begin(), ite = key_order.end(); it != ite; ++it, ++pos)
            {
                const yaml_value* key = it->get();
                if (key->type != yaml_node_t::string)
                    throw yaml_document_error("JSON doesn't support non-string key.");

                auto val_pos = vals.find(key);
                assert(val_pos != vals.end());

                dump_json_item(
                    os,
                    &static_cast<const yaml_value_string*>(key)->value_string,
                    *val_pos->second, scope, pos < (n-1));
            }

            dump_indent(os, scope);
            os << "}";
        }
        break;
        case yaml_node_t::sequence:
        {
            auto& vals = static_cast<const yaml_value_sequence&>(node).value_sequence;
            os << "[" << std::endl;
            size_t n = vals.size();
            size_t pos = 0;
            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it, ++pos)
                dump_json_item(os, nullptr, **it, scope, pos < (n-1));

            dump_indent(os, scope);
            os << "]";
        }
        break;
        case yaml_node_t::boolean_true:
            os << kw_true;
        break;
        case yaml_node_t::boolean_false:
            os << kw_false;
        break;
        case yaml_node_t::null:
            os << kw_null;
        break;
        case yaml_node_t::number:
            os << static_cast<const yaml_value_number&>(node).value_number;
        break;
        case yaml_node_t::string:
            json::dump_string(os, static_cast<const yaml_value_string&>(node).value_string);
        break;
        case yaml_node_t::unset:
        default:
            ;
    }
}

const char* warning_multiple_documents =
"warning: this YAML file contains multiple documents.  Only the first document\n"
"will be written.";

}

std::string yaml_document_tree::dump_yaml() const
{
    std::ostringstream os;

    std::for_each(
        mp_impl->m_docs.begin(), mp_impl->m_docs.end(),
        [&](const document_root_type& root)
        {
            dump_yaml_document(os, *root);
        }
    );

    return os.str();
}

std::string yaml_document_tree::dump_json() const
{
    if (mp_impl->m_docs.empty())
        return std::string();

    if (mp_impl->m_docs.size() > 1)
        std::cerr << warning_multiple_documents << std::endl;

    const yaml_value& root = *mp_impl->m_docs.front();

    std::ostringstream os;
    dump_json_node(os, root, 0);

    os << std::endl;

    return os.str();
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
