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

#include <boost/noncopyable.hpp>

namespace orcus { namespace model {

/**
 * This class handles global pool of string instances.
 */
class shared_strings : public interface::shared_strings, private ::boost::noncopyable
{
    typedef ::std::unordered_map<pstring, size_t, pstring::hash> str_index_map_type;

public:
    struct format_run
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

    // format runs for single string
    typedef ::std::vector<format_run> format_runs_type;
    // format runs for all shared strings, mapped by string IDs.
    typedef ::std::unordered_map<size_t, format_runs_type*> format_runs_map_type;

    shared_strings();
    virtual ~shared_strings();

    virtual size_t append(const char* s, size_t n);
    virtual size_t add(const char* s, size_t n);

    virtual void set_segment_bold(bool b);
    virtual void set_segment_italic(bool b);
    virtual void set_segment_font_name(const char* s, size_t n);
    virtual void set_segment_font_size(double point);
    virtual void append_segment(const char* s, size_t n);
    virtual size_t commit_segments();

    bool has(size_t index) const;
    const pstring& get(size_t index) const;
    const format_runs_type* get_format_runs(size_t index) const;

    void dump() const;


private:
    size_t append_to_pool(const pstring& ps);

private:
    /**
     * String pool, contains only simple unformatted strings.
     */
    ::std::vector<pstring> m_strings;

    /**
     * Container for all format runs of all formatted strings.  Format runs
     * are mapped with the string IDs.
     */
    format_runs_map_type m_formats;

    ::std::string   m_cur_segment_string;
    format_run      m_cur_format;
    format_runs_type*   mp_cur_format_runs;
    str_index_map_type m_set;
};

}}

#endif
