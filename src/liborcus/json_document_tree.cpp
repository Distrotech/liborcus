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
#include "orcus/config.hpp"
#include "orcus/stream.hpp"
#include "orcus/string_pool.hpp"

#include "json_util.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <limits>

#include <boost/current_function.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace orcus {

json_document_error::json_document_error(const std::string& msg) :
    general_error("json_document_error", msg) {}

json_document_error::~json_document_error() throw() {}

namespace json { namespace detail {

struct json_value
{
    node_t type;
    json_value* parent;

    json_value() : type(node_t::unset), parent(nullptr) {}
    json_value(node_t _type) : type(_type), parent(nullptr) {}
    virtual ~json_value() {}
};

}}

namespace {

using json_value = json::detail::json_value;
using node_t = json::detail::node_t;

const char* tab = "    ";
constexpr char quote = '"';
constexpr char backslash = '\\';

const xmlns_id_t NS_orcus_json_xml = "http://schemas.kohei.us/orcus/2015/json";

struct json_value_string : public json_value
{
    pstring value_string;

    json_value_string() : json_value(node_t::string) {}
    json_value_string(const pstring& s) : json_value(node_t::string), value_string(s) {}
    virtual ~json_value_string() {}
};

struct json_value_number : public json_value
{
    double value_number;

    json_value_number() :
        json_value(node_t::number),
        value_number(std::numeric_limits<double>::quiet_NaN()) {}

    json_value_number(double num) : json_value(node_t::number), value_number(num) {}

    virtual ~json_value_number() {}
};

struct json_value_array : public json_value
{
    std::vector<std::unique_ptr<json_value>> value_array;

    json_value_array() : json_value(node_t::array) {}
    virtual ~json_value_array() {}
};

struct json_value_object : public json_value
{
    using object_type = std::unordered_map<pstring, std::unique_ptr<json_value>, pstring::hash>;

    std::vector<pstring> key_order;
    object_type value_object;

    bool has_ref;

    json_value_object() : json_value(node_t::object), has_ref(false) {}
    virtual ~json_value_object() {}

    void swap(json_value_object& src)
    {
        key_order.swap(src.key_order);
        value_object.swap(src.value_object);
    }
};

void dump_repeat(std::ostringstream& os, const char* s, int repeat)
{
    for (int i = 0; i < repeat; ++i)
        os << s;
}

void dump_item(
    std::ostringstream& os, const pstring* key, const json_value* val,
    int level, bool sep);

void dump_value(std::ostringstream& os, const json_value* v, int level, const pstring* key = nullptr)
{
    dump_repeat(os, tab, level);

    if (key)
        os << quote << *key << quote << ": ";

    switch (v->type)
    {
        case node_t::array:
        {
            auto& vals = static_cast<const json_value_array*>(v)->value_array;
            os << "[" << std::endl;
            size_t n = vals.size();
            size_t pos = 0;
            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it, ++pos)
                dump_item(os, nullptr, it->get(), level, pos < (n-1));

            dump_repeat(os, tab, level);
            os << "]";
        }
        break;
        case node_t::boolean_false:
            os << "false";
        break;
        case node_t::boolean_true:
            os << "true";
        break;
        case node_t::null:
            os << "null";
        break;
        case node_t::number:
            os << static_cast<const json_value_number*>(v)->value_number;
        break;
        case node_t::object:
        {
            const std::vector<pstring>& key_order = static_cast<const json_value_object*>(v)->key_order;
            auto& vals = static_cast<const json_value_object*>(v)->value_object;
            os << "{" << std::endl;
            size_t n = vals.size();

            if (key_order.empty())
            {
                // Dump object's children unordered.
                size_t pos = 0;
                for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it, ++pos)
                {
                    const pstring& key = it->first;
                    auto& val = it->second;

                    dump_item(os, &key, val.get(), level, pos < (n-1));
                }
            }
            else
            {
                // Dump them based on key's original ordering.
                size_t pos = 0;
                for (auto it = key_order.begin(), ite = key_order.end(); it != ite; ++it, ++pos)
                {
                    const pstring& key = *it;
                    auto val_pos = vals.find(key);
                    assert(val_pos != vals.end());

                    dump_item(os, &key, val_pos->second.get(), level, pos < (n-1));
                }
            }

            dump_repeat(os, tab, level);
            os << "}";
        }
        break;
        case node_t::string:
            json::dump_string(os, static_cast<const json_value_string*>(v)->value_string.str());
        break;
        case node_t::unset:
        default:
            ;
    }
}

void dump_item(
    std::ostringstream& os, const pstring* key, const json_value* val,
    int level, bool sep)
{
    dump_value(os, val, level+1, key);
    if (sep)
        os << ",";
    os << std::endl;
}

std::string dump_json_tree(const json_value* root)
{
    if (root->type == node_t::unset)
        return std::string();

    std::ostringstream os;
    dump_value(os, root, 0);
    return os.str();
}

void dump_string_xml(std::ostringstream& os, const pstring& s)
{
    const char* p = s.get();
    const char* p_end = p + s.size();
    for (; p != p_end; ++p)
    {
        char c = *p;
        switch (c)
        {
            case '"':
                os << "&quot;";
            break;
            case '<':
                os << "&lt;";
            break;
            case '>':
                os << "&gt;";
            break;
            case '&':
                os << "&amp;";
            break;
            case '\'':
                os << "&apos;";
            break;
            default:
                os << c;
        }
    }
}

void dump_object_item_xml(
    std::ostringstream& os, const pstring& key, const json_value* val, int level);

void dump_value_xml(std::ostringstream& os, const json_value* v, int level)
{
    switch (v->type)
    {
        case node_t::array:
        {
            os << "<array";
            if (level == 0)
                os << " xmlns=\"" << NS_orcus_json_xml << "\"";
            os << ">";

            auto& vals = static_cast<const json_value_array*>(v)->value_array;

            for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it)
            {
                os << "<item>";
                dump_value_xml(os, it->get(), level+1);
                os << "</item>";
            }

            os << "</array>";
        }
        break;
        case node_t::boolean_false:
            os << "<false/>";
        break;
        case node_t::boolean_true:
            os << "<true/>";
        break;
        case node_t::null:
            os << "<null/>";
        break;
        case node_t::number:
            os << "<number value=\"";
            os << static_cast<const json_value_number*>(v)->value_number;
            os << "\"/>";
        break;
        case node_t::object:
        {
            os << "<object";
            if (level == 0)
                os << " xmlns=\"" << NS_orcus_json_xml << "\"";
            os << ">";

            auto& key_order = static_cast<const json_value_object*>(v)->key_order;
            auto& vals = static_cast<const json_value_object*>(v)->value_object;

            if (key_order.empty())
            {
                // Dump object's children unordered.
                for (auto it = vals.begin(), ite = vals.end(); it != ite; ++it)
                {
                    auto& key = it->first;
                    auto& val = it->second;
                    dump_object_item_xml(os, key, val.get(), level);
                }
            }
            else
            {
                // Dump them based on key's original ordering.
                for (auto it = key_order.begin(), ite = key_order.end(); it != ite; ++it)
                {
                    auto& key = *it;
                    auto val_pos = vals.find(key);
                    assert(val_pos != vals.end());

                    dump_object_item_xml(os, key, val_pos->second.get(), level);
                }
            }

            os << "</object>";
        }
        break;
        case node_t::string:
            os << "<string value=\"";
            dump_string_xml(os, static_cast<const json_value_string*>(v)->value_string);
            os << "\"/>";
        break;
        case node_t::unset:
        default:
            ;
    }
}

void dump_object_item_xml(
    std::ostringstream& os, const pstring& key, const json_value* val, int level)
{
    os << "<item name=\"";
    dump_string_xml(os, key);
    os << "\">";
    dump_value_xml(os, val, level+1);
    os << "</item>";
}

std::string dump_xml_tree(const json_value* root)
{
    if (root->type == node_t::unset)
        return std::string();

    std::ostringstream os;
    os << "<?xml version=\"1.0\"?>" << std::endl;
    dump_value_xml(os, root, 0);
    os << std::endl;
    return os.str();
}

struct parser_stack
{
    pstring key;
    json_value* node;

    parser_stack(json_value* _node) : node(_node) {}
};

struct external_ref
{
    pstring path;
    json_value_object* dest;

    external_ref(const pstring& _path, json_value_object* _dest) :
        path(_path), dest(_dest) {}
};

class parser_handler
{
    const json_config& m_config;

    std::unique_ptr<json_value> m_root;
    std::vector<parser_stack> m_stack;
    std::vector<external_ref> m_external_refs;

    string_pool& m_pool;

    json_value* push_value(std::unique_ptr<json_value>&& value)
    {
        assert(!m_stack.empty());
        parser_stack& cur = m_stack.back();

        switch (cur.node->type)
        {
            case node_t::array:
            {
                json_value_array* jva = static_cast<json_value_array*>(cur.node);
                value->parent = jva;
                jva->value_array.push_back(std::move(value));
                return jva->value_array.back().get();
            }
            break;
            case node_t::object:
            {
                const pstring& key = cur.key;
                json_value_object* jvo = static_cast<json_value_object*>(cur.node);
                value->parent = jvo;

                if (m_config.resolve_references &&
                    key == "$ref" && value->type == node_t::string)
                {
                    json_value_string* jvs = static_cast<json_value_string*>(value.get());
                    if (!jvo->has_ref && !jvs->value_string.empty() && jvs->value_string[0] != '#')
                    {
                        // Store the external reference path and the destination
                        // object for later processing.
                        m_external_refs.emplace_back(jvs->value_string, jvo);
                        jvo->has_ref = true;
                    }
                }

                if (m_config.preserve_object_order)
                    jvo->key_order.push_back(key);

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
    parser_handler(const json_config& config, string_pool& pool) :
        m_config(config), m_pool(pool) {}

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
            assert(jv && jv->type == node_t::array);
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
            assert(jv && jv->type == node_t::object);
            m_stack.push_back(parser_stack(jv));
        }
        else
        {
            m_root = make_unique<json_value_object>();
            m_stack.push_back(parser_stack(m_root.get()));
        }
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        parser_stack& cur = m_stack.back();
        cur.key = pstring(p, len);
        if (m_config.persistent_string_values || transient)
            // The tree manages the life cycle of this string value.
            cur.key = m_pool.intern(cur.key).first;
    }

    void end_object()
    {
        assert(!m_stack.empty());
        m_stack.pop_back();
    }

    void boolean_true()
    {
        push_value(make_unique<json_value>(node_t::boolean_true));
    }

    void boolean_false()
    {
        push_value(make_unique<json_value>(node_t::boolean_false));
    }

    void null()
    {
        push_value(make_unique<json_value>(node_t::null));
    }

    void string(const char* p, size_t len, bool transient)
    {
        pstring s(p, len);
        if (m_config.persistent_string_values || transient)
            // The tree manages the life cycle of this string value.
            s = m_pool.intern(s).first;

        push_value(make_unique<json_value_string>(s));
    }

    void number(double val)
    {
        push_value(make_unique<json_value_number>(val));
    }

    void swap(std::unique_ptr<json_value>& other_root)
    {
        other_root.swap(m_root);
    }

    const std::vector<external_ref>& get_external_refs() const
    {
        return m_external_refs;
    }
};

}

namespace json { namespace detail {

struct node::impl
{
    const json_value* m_node;

    impl(const json_value* jv) : m_node(jv) {}
};

node::node(const json_value* jv) : mp_impl(make_unique<impl>(jv)) {}
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
        case node_t::object:
            return static_cast<const json_value_object*>(mp_impl->m_node)->value_object.size();
        case node_t::array:
            return static_cast<const json_value_array*>(mp_impl->m_node)->value_array.size();
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

std::vector<pstring> node::keys() const
{
    if (mp_impl->m_node->type != node_t::object)
        throw json_document_error("node::keys: this node is not of object type.");

    const json_value_object* jvo = static_cast<const json_value_object*>(mp_impl->m_node);
    std::vector<pstring> keys;
    std::for_each(jvo->value_object.begin(), jvo->value_object.end(),
        [&](const json_value_object::object_type::value_type& node)
        {
            keys.push_back(node.first);
        }
    );

    return keys;
}

pstring node::key(size_t index) const
{
    if (mp_impl->m_node->type != node_t::object)
        throw json_document_error("node::key: this node is not of object type.");

    const json_value_object* jvo = static_cast<const json_value_object*>(mp_impl->m_node);
    if (index >= jvo->key_order.size())
        throw std::out_of_range("node::key: index is out-of-range.");

    return jvo->key_order[index];
}

node node::child(size_t index) const
{
    switch (mp_impl->m_node->type)
    {
        case node_t::object:
        {
            // This works only when the key order is preserved.
            const json_value_object* jvo = static_cast<const json_value_object*>(mp_impl->m_node);
            if (index >= jvo->key_order.size())
                throw std::out_of_range("node::child: index is out-of-range");

            const pstring& key = jvo->key_order[index];
            auto it = jvo->value_object.find(key);
            assert(it != jvo->value_object.end());
            return node(it->second.get());
        }
        break;
        case node_t::array:
        {
            const json_value_array* jva = static_cast<const json_value_array*>(mp_impl->m_node);
            if (index >= jva->value_array.size())
                throw std::out_of_range("node::child: index is out-of-range");

            return node(jva->value_array[index].get());
        }
        break;
        case node_t::string:
        case node_t::number:
        case node_t::boolean_true:
        case node_t::boolean_false:
        case node_t::null:
        case node_t::unset:
        default:
            throw json_document_error("node::child: this node cannot have child nodes.");
    }
}

node node::child(const pstring& key) const
{
    if (mp_impl->m_node->type != node_t::object)
        throw json_document_error("node::child: this node is not of object type.");

    const json_value_object* jvo = static_cast<const json_value_object*>(mp_impl->m_node);
    auto it = jvo->value_object.find(key);
    if (it == jvo->value_object.end())
    {
        std::ostringstream os;
        os << "node::child: this object does not have a key labeled '" << key << "'";
        throw json_document_error(os.str());
    }

    return node(it->second.get());
}

node node::parent() const
{
    if (!mp_impl->m_node->parent)
        throw json_document_error("node::parent: this node has no parent.");

    return node(mp_impl->m_node->parent);
}

pstring node::string_value() const
{
    if (mp_impl->m_node->type != node_t::string)
        throw json_document_error("node::key: current node is not of string type.");

    const json_value_string* jvs = static_cast<const json_value_string*>(mp_impl->m_node);
    return jvs->value_string;
}

double node::numeric_value() const
{
    if (mp_impl->m_node->type != node_t::number)
        throw json_document_error("node::key: current node is not of numeric type.");

    const json_value_number* jvn = static_cast<const json_value_number*>(mp_impl->m_node);
    return jvn->value_number;
}

}}

struct json_document_tree::impl
{
    std::unique_ptr<json_value> m_root;
    std::unique_ptr<string_pool> m_own_pool;
    string_pool& m_pool;

    impl() : m_own_pool(make_unique<string_pool>()), m_pool(*m_own_pool) {}
    impl(string_pool& pool) : m_pool(pool) {}
};

json_document_tree::json_document_tree() : mp_impl(make_unique<impl>()) {}
json_document_tree::json_document_tree(string_pool& pool) : mp_impl(make_unique<impl>(pool)) {}
json_document_tree::~json_document_tree() {}

void json_document_tree::load(const std::string& strm, const json_config& config)
{
    load(strm.data(), strm.size(), config);
}

void json_document_tree::load(const char* p, size_t n, const json_config& config)
{
    parser_handler hdl(config, mp_impl->m_pool);
    json_parser<parser_handler> parser(p, n, hdl);
    parser.parse();
    hdl.swap(mp_impl->m_root);

    auto& external_refs = hdl.get_external_refs();

    json_config ext_config = config;
    // The stream will get destroyed after each parsing of an external json file.
    ext_config.persistent_string_values = true;

    fs::path parent_dir = config.input_path;
    parent_dir = parent_dir.parent_path();
    for (auto it = external_refs.begin(), ite = external_refs.end(); it != ite; ++it)
    {
        fs::path extfile = it->path.str();
        fs::path extpath = parent_dir;
        extpath /= extfile;

        // Get the stream content from the path.
        std::string ext_strm = load_file_content(extpath.string().c_str());

        ext_config.input_path = extpath.string();
        json_document_tree doc(mp_impl->m_pool);
        doc.load(ext_strm, ext_config);

        json_value* root = doc.mp_impl->m_root.get();
        if (root->type == node_t::object)
        {
            json_value_object* jvo_src = static_cast<json_value_object*>(root);
            json_value_object* jvo_dest = it->dest;
            if (jvo_dest->value_object.size() == 1)
            {
                // Swap with the referenced object only when the destination
                // has one child value i.e. it only has '$ref'.
                jvo_dest->swap(*jvo_src);
                jvo_dest->has_ref = false;
            }
        }
    }
}

json_document_tree::node json_document_tree::get_document_root() const
{
    return node(mp_impl->m_root.get());
}

std::string json_document_tree::dump() const
{
    if (!mp_impl->m_root)
        return std::string();

    return dump_json_tree(mp_impl->m_root.get());
}

std::string json_document_tree::dump_xml() const
{
    return dump_xml_tree(mp_impl->m_root.get());
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
