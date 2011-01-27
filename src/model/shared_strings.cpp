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

#include "orcus/model/shared_strings.hpp"
#include "orcus/pstring.hpp"
#include "orcus/global.hpp"

#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

namespace orcus { namespace model {

shared_strings::format_run::format_run() :
    pos(0), size(0), 
    font_size(0),
    bold(false), italic(false) {}

void shared_strings::format_run::reset()
{
    pos = 0;
    size = 0;
    font.clear();
    font_size = 0;
    bold = false;
    italic = false;
}

bool shared_strings::format_run::formatted() const
{
    if (bold || italic)
        return true;

    if (font_size)
        return true;

    if (!font.empty())
        return true;

    return false;
}

shared_strings::shared_strings() :
    mp_cur_format_runs(NULL)
{
}

shared_strings::~shared_strings()
{
    for_each(m_formats.begin(), m_formats.end(), 
             delete_map_object<format_runs_map_type>());

    // This pointer should be NULL.
    assert(!mp_cur_format_runs);
    delete mp_cur_format_runs;
}

size_t shared_strings::append(const char* s, size_t n)
{
    pstring ps = pstring(s, n).intern();
    return append_to_pool(ps);
}

size_t shared_strings::add(const char* s, size_t n)
{
    pstring ps = pstring(s, n).intern();

    // Check if this string is already in the pool.
    str_index_map_type::const_iterator itr = m_set.find(ps);
    if (itr != m_set.end())
    {
        // It's already in the pool.
        return itr->second;
    }

    // Not in the pool yet.  Insert it into the pool.
    return append_to_pool(ps);
}

size_t shared_strings::append_to_pool(const pstring& ps)
{
    size_t index = m_strings.size();
    m_strings.push_back(ps);
    m_set.insert(str_index_map_type::value_type(ps, index));
    return index;
}

bool shared_strings::has(size_t index) const
{
    return index < m_strings.size();
}

const pstring& shared_strings::get(size_t index) const
{
    return m_strings[index];
}

const shared_strings::format_runs_type* shared_strings::get_format_runs(size_t index) const
{
    format_runs_map_type::const_iterator itr = m_formats.find(index);
    if (itr != m_formats.end())
        return itr->second;
    return NULL;
}

void shared_strings::set_segment_bold(bool b)
{
    m_cur_format.bold = b;
}

void shared_strings::set_segment_italic(bool b)
{
    m_cur_format.italic = b;
}

void shared_strings::set_segment_font_name(const char* s, size_t n)
{
    m_cur_format.font = pstring(s, n).intern();
}

void shared_strings::set_segment_font_size(double point)
{
    m_cur_format.font_size = point;
}

void shared_strings::append_segment(const char* s, size_t n)
{
    if (!n)
        return;

    size_t start_pos = m_cur_segment_string.size();
    m_cur_segment_string += string(s, n);

    if (m_cur_format.formatted())
    {
        // This segment is formatted.
        // Record the position and size of the format run.
        m_cur_format.pos = start_pos;
        m_cur_format.size = n;
        
        if (!mp_cur_format_runs)
            mp_cur_format_runs = new format_runs_type;
    
        mp_cur_format_runs->push_back(m_cur_format);
        m_cur_format.reset();
    }
}

size_t shared_strings::commit_segments()
{
    pstring ps = pstring(m_cur_segment_string.data(), m_cur_segment_string.size()).intern();
    m_cur_segment_string.clear();
    size_t sindex = append_to_pool(ps);
    m_formats.insert(format_runs_map_type::value_type(sindex, mp_cur_format_runs));
    mp_cur_format_runs = NULL;
    return sindex;
}

namespace {

struct print_string : public unary_function<void, pstring>
{
    size_t m_count;
public:
    print_string() : m_count(1) {}
    void operator() (const pstring& ps)
    {
        cout << m_count++ << ": '" << ps << "'" << endl;
    }
};

}

void shared_strings::dump() const
{
    cout << "number of shared strings: " << m_strings.size() << endl;
//  for_each(m_strings.begin(), m_strings.end(), print_string());
}

}}
