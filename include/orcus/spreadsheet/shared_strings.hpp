/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_SPREADSHEET_SHARED_STRINGS_HPP
#define INCLUDED_ORCUS_SPREADSHEET_SHARED_STRINGS_HPP

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/spreadsheet/styles.hpp"
#include "orcus/pstring.hpp"
#include "orcus/env.hpp"

#include <cstdlib>
#include <vector>
#include <unordered_map>

namespace ixion { class model_context; }

namespace orcus {

class string_pool;

namespace spreadsheet {

class import_styles;

struct ORCUS_SPM_DLLPUBLIC format_run
{
    size_t pos;
    size_t size;
    pstring font;
    double font_size;
    color_t color;
    bool bold:1;
    bool italic:1;

    format_run();

    void reset();
    bool formatted() const;
};

typedef std::vector<format_run> format_runs_t;

/**
 * This class handles global pool of string instances.
 */
class ORCUS_DLLPUBLIC import_shared_strings : public iface::import_shared_strings
{
    typedef std::unordered_map<pstring, size_t, pstring::hash> str_index_map_type;

    import_shared_strings() = delete;
    import_shared_strings(const import_shared_strings&) = delete;
    import_shared_strings& operator=(const import_shared_strings&) = delete;

public:

    // format runs for all shared strings, mapped by string IDs.
    typedef std::unordered_map<size_t, format_runs_t*> format_runs_map_type;

    import_shared_strings(orcus::string_pool& sp, ixion::model_context& cxt, import_styles& styles);
    virtual ~import_shared_strings();

    virtual size_t append(const char* s, size_t n);
    virtual size_t add(const char* s, size_t n);

    virtual void set_segment_font(size_t font_index);
    virtual void set_segment_bold(bool b);
    virtual void set_segment_italic(bool b);
    virtual void set_segment_font_name(const char* s, size_t n);
    virtual void set_segment_font_size(double point);
    virtual void set_segment_font_color(color_elem_t alpha, color_elem_t red, color_elem_t green, color_elem_t blue);
    virtual void append_segment(const char* s, size_t n);
    virtual size_t commit_segments();

    const format_runs_t* get_format_runs(size_t index) const;

    const std::string* get_string(size_t index) const;

    void dump() const;

private:
    orcus::string_pool& m_string_pool;
    ixion::model_context& m_cxt;
    import_styles& m_styles;

    /**
     * Container for all format runs of all formatted strings.  Format runs
     * are mapped with the string IDs.
     */
    format_runs_map_type m_formats;

    ::std::string   m_cur_segment_string;
    format_run      m_cur_format;
    format_runs_t* mp_cur_format_runs;
    str_index_map_type m_set;
};

}}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
