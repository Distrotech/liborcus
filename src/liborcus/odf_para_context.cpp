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

#include "orcus/odf/para_context.hpp"
#include "orcus/odf/odf_token_constants.hpp"
#include "orcus/spreadsheet/interface.hpp"

#include <iostream>
#include <cassert>

using namespace std;

namespace orcus {

text_para_context::text_para_context(const tokens& tokens, spreadsheet::iface::shared_strings* ssb) :
    xml_context_base(tokens),
    mp_sstrings(ssb),
    m_string_index(0),
    m_formatted(false)
{
}

text_para_context::~text_para_context()
{
}

bool text_para_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* text_para_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void text_para_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
    // not implemented yet.
}

void text_para_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == XMLNS_text)
    {
        switch (name)
        {
            case XML_p:
                // paragraph
                xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);
                m_formatted = false;
            break;
            case XML_span:
                // text span.
                xml_element_expected(parent, XMLNS_text, XML_p);
                m_formatted = true;
            break;
            case XML_s:
                // control character.  ignored for now.
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool text_para_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    if (ns == XMLNS_text && name == XML_p)
    {
        // paragraph
        if (m_formatted)
        {
            // this paragraph consists of several segments, some of which may
            // be formatted.

            vector<pstring>::const_iterator itr = m_contents.begin(), itr_end = m_contents.end();
            for (; itr != itr_end; ++itr)
            {
                const pstring& ps = *itr;
                mp_sstrings->append_segment(ps.get(), ps.size());
            }
            m_string_index = mp_sstrings->commit_segments();
        }
        else if (!m_contents.empty())
        {
            // Unformatted simple text paragraph.  We may still get several
            // segments in presence of control characters separating the
            // paragraph text.

            vector<pstring>::const_iterator itr = m_contents.begin(), itr_end = m_contents.end();
            for (; itr != itr_end; ++itr)
            {
                const pstring& ps = *itr;
                mp_sstrings->append_segment(ps.get(), ps.size());
            }
            m_string_index = mp_sstrings->commit_segments();
        }
    }
    else if (ns == XMLNS_text && name == XML_span)
    {
        // text span.
    }
    return pop_stack(ns, name);
}

void text_para_context::characters(const pstring& str)
{
    m_contents.push_back(str);
}

size_t text_para_context::get_string_index() const
{
    return m_string_index;
}

bool text_para_context::empty() const
{
    return m_contents.empty();
}

}
