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

#ifndef ORCUS_PARACONTEXT_HPP
#define ORCUS_PARACONTEXT_HPP

#include "xml_context_base.hpp"
#include "odf_styles.hpp"

#include "orcus/pstring.hpp"
#include "orcus/string_pool.hpp"

#include <vector>

namespace orcus {

namespace spreadsheet { namespace iface { class import_shared_strings; }}

/**
 * This class handles <text:p> contexts.
 */
class text_para_context : public xml_context_base
{
public:
    text_para_context(
       session_context& session_cxt, const tokens& tokens,
       spreadsheet::iface::import_shared_strings* ssb, odf_styles_map_type& styles);
    virtual ~text_para_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

    void reset();

    size_t get_string_index() const;
    bool empty() const;

private:
    void flush_segment();

private:
    spreadsheet::iface::import_shared_strings* mp_sstrings;
    odf_styles_map_type& m_styles;

    string_pool m_pool;
    std::vector<pstring> m_span_stack; /// stack of text spans.
    std::vector<pstring> m_contents;
    size_t m_string_index;
    bool m_has_content;
};

}

#endif
