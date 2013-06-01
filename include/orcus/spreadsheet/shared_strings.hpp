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

#ifndef __ORCUS_SPREADSHEET_SHARED_STRINGS_HPP__
#define __ORCUS_SPREADSHEET_SHARED_STRINGS_HPP__

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/pstring.hpp"
#include "orcus/env.hpp"

#include <cstdlib>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

namespace ixion { class model_context; }

namespace orcus {

class string_pool;

namespace spreadsheet {

struct ORCUS_DLLPUBLIC format_run
{
    size_t pos;
    size_t size;
    pstring font;
    double font_size;
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
class ORCUS_DLLPUBLIC import_shared_strings : public iface::import_shared_strings, private boost::noncopyable
{
    typedef boost::unordered_map<pstring, size_t, pstring::hash> str_index_map_type;

    import_shared_strings(); // disabled

public:

    // format runs for all shared strings, mapped by string IDs.
    typedef boost::unordered_map<size_t, format_runs_t*> format_runs_map_type;

    import_shared_strings(orcus::string_pool& sp, ixion::model_context& cxt);
    virtual ~import_shared_strings();

    virtual size_t append(const char* s, size_t n);
    virtual size_t add(const char* s, size_t n);

    virtual void set_segment_bold(bool b);
    virtual void set_segment_italic(bool b);
    virtual void set_segment_font_name(const char* s, size_t n);
    virtual void set_segment_font_size(double point);
    virtual void append_segment(const char* s, size_t n);
    virtual size_t commit_segments();

    const format_runs_t* get_format_runs(size_t index) const;

    void dump() const;

private:
    orcus::string_pool& m_string_pool;
    ixion::model_context& m_cxt;

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
