/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/spreadsheet/shared_strings.hpp"
#include "orcus/spreadsheet/styles.hpp"

#include "orcus/pstring.hpp"
#include "orcus/global.hpp"
#include "orcus/string_pool.hpp"

#include <ixion/model_context.hpp>

#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

namespace orcus { namespace spreadsheet {

format_run::format_run() :
    pos(0), size(0),
    font_size(0),
    bold(false), italic(false) {}

void format_run::reset()
{
    pos = 0;
    size = 0;
    font.clear();
    font_size = 0;
    bold = false;
    italic = false;
    color = color_t();
}

bool format_run::formatted() const
{
    if (bold || italic)
        return true;

    if (font_size)
        return true;

    if (!font.empty())
        return true;

    return false;
}

import_shared_strings::import_shared_strings(orcus::string_pool& sp, ixion::model_context& cxt, import_styles& styles) :
    m_string_pool(sp), m_cxt(cxt), m_styles(styles), mp_cur_format_runs(NULL) {}

import_shared_strings::~import_shared_strings()
{
    for_each(m_formats.begin(), m_formats.end(),
             map_object_deleter<format_runs_map_type>());

    // This pointer should be NULL.
    assert(!mp_cur_format_runs);
    delete mp_cur_format_runs;
}

size_t import_shared_strings::append(const char* s, size_t n)
{
    return m_cxt.append_string(s, n);
}

size_t import_shared_strings::add(const char* s, size_t n)
{
    return m_cxt.add_string(s, n);
}

const format_runs_t* import_shared_strings::get_format_runs(size_t index) const
{
    format_runs_map_type::const_iterator itr = m_formats.find(index);
    if (itr != m_formats.end())
        return itr->second;
    return NULL;
}

const string* import_shared_strings::get_string(size_t index) const
{
    return m_cxt.get_string(index);
}

void import_shared_strings::set_segment_font(size_t font_index)
{
    const font_t* font_data = m_styles.get_font(font_index);
    if (!font_data)
        return;

    m_cur_format.bold = font_data->bold;
    m_cur_format.italic = font_data->italic;
    m_cur_format.font = font_data->name; // font names are already interned when set.
    m_cur_format.font_size = font_data->size;
    m_cur_format.color = font_data->color;
}

void import_shared_strings::set_segment_bold(bool b)
{
    m_cur_format.bold = b;
}

void import_shared_strings::set_segment_italic(bool b)
{
    m_cur_format.italic = b;
}

void import_shared_strings::set_segment_font_name(const char* s, size_t n)
{
    m_cur_format.font = m_string_pool.intern(s, n).first;
}

void import_shared_strings::set_segment_font_size(double point)
{
    m_cur_format.font_size = point;
}

void import_shared_strings::set_segment_font_color(
    color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue)
{
    m_cur_format.color = color_t(alpha, red, green, blue);
}

void import_shared_strings::append_segment(const char* s, size_t n)
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
            mp_cur_format_runs = new format_runs_t;

        mp_cur_format_runs->push_back(m_cur_format);
        m_cur_format.reset();
    }
}

size_t import_shared_strings::commit_segments()
{
    size_t sindex = m_cxt.append_string(m_cur_segment_string.data(), m_cur_segment_string.size());
    m_cur_segment_string.clear();
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

void import_shared_strings::dump() const
{
    cout << "number of shared strings: " << m_cxt.get_string_count() << endl;
}

}}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
