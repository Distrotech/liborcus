/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "ooxml/ooxml_tokens.hpp"
#include "global.hpp"

#include <cstdlib>
#include <string>
#include <unordered_map>

using namespace std;

namespace orcus {

namespace {

#include "ooxml_tokens.inl"

struct string_hash
{
    size_t operator() (const string& val) const
    {
        size_t n = val.size();
        size_t hash_val = ~n;
        size_t loop_size = min<size_t>(n, 20); // prevent too much looping.
        for (size_t i = 0; i < loop_size; ++i)
            hash_val += static_cast<size_t>(val[i]);

        return hash_val;
    }
};

struct name_token_map
{
    typedef ::std::unordered_map<string, xml_token_t, string_hash> token_type;
    typedef ::std::unordered_map<string, xmlns_token_t, string_hash> nstoken_type;

    static token_type       tokens;
    static nstoken_type     nstokens;
    static pthread_mutex_t  lock;
};

name_token_map::token_type   name_token_map::tokens;
name_token_map::nstoken_type name_token_map::nstokens;
pthread_mutex_t name_token_map::lock;

}

void tokens::init()
{
    thread_mutex_guard guard(name_token_map::lock);

    for (size_t i = 0; i < token_name_count; ++i)
    {
        name_token_map::tokens.insert(
            name_token_map::token_type::value_type(
                string(token_names[i]), static_cast<xml_token_t>(i)));
    }

    for (size_t i = 0; i < nstoken_name_count; ++i)
    {
        name_token_map::nstokens.insert(
            name_token_map::nstoken_type::value_type(
                string(nstoken_names[i]), static_cast<xmlns_token_t>(i)));
    }
}

bool tokens::is_valid_token(xml_token_t token)
{
    return token != XML_UNKNOWN_TOKEN;
}

xml_token_t tokens::get_token(const string& name)
{
    name_token_map::token_type::const_iterator itr = name_token_map::tokens.find(name);
    if (itr == name_token_map::tokens.end())
        return XML_UNKNOWN_TOKEN;
    return itr->second;
}

const char* tokens::get_token_name(xml_token_t token)
{
    if (static_cast<size_t>(token) >= token_name_count)
        return "";

    return token_names[token];
}

bool tokens::is_valid_nstoken(xmlns_token_t token)
{
    return token != XMLNS_UNKNOWN_TOKEN;
}

xmlns_token_t tokens::get_nstoken(const string& name)
{
    name_token_map::nstoken_type::const_iterator itr = name_token_map::nstokens.find(name);
    if (itr == name_token_map::nstokens.end())
        return XMLNS_UNKNOWN_TOKEN;
    return itr->second;
}

const char* tokens::get_nstoken_name(xmlns_token_t token)
{
    if (static_cast<size_t>(token) >= nstoken_name_count)
        return "";

    return nstoken_names[token];
}

}
