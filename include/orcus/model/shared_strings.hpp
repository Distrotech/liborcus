/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
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

#ifndef __ORCUS_MODEL_SHARED_STRINGS_HPP__
#define __ORCUS_MODEL_SHARED_STRINGS_HPP__

#include "orcus/model/interface.hpp"
#include "orcus/pstring.hpp"

#include <cstdlib>
#include <vector>
#include <unordered_map>

namespace orcus { namespace model {

/**
 * This class handles global pool of string instances.
 */
class shared_strings : public shared_strings_base
{
    typedef ::std::unordered_map<pstring, size_t, pstring::hash> str_index_map_type;

public:
    shared_strings();
    virtual ~shared_strings();

    virtual size_t append(const char* s, size_t n);
    virtual size_t add(const char* s, size_t n);
    virtual void append_segment(const char* s, size_t n);
    virtual size_t commit_segments();

    bool has(size_t index) const;
    const pstring& get(size_t index) const;
    void dump() const;

private:
    size_t append_to_pool(const pstring& ps);
private:
    ::std::vector<pstring> m_strings;
    ::std::string m_segment_buffer;
    str_index_map_type m_set;
};

}}

#endif
