/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
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

#ifndef ORCUS_ODF_STYLES_CONTEXT_HPP
#define ORCUS_ODF_STYLES_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "odf_styles.hpp"

#include "orcus/global.hpp"

#include <boost/unordered_map.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {
    class import_factory;
}}

class style_value_converter
{
    typedef boost::unordered_map<pstring, odf_style_family, pstring::hash> style_families_type;
    style_families_type m_style_families;

public:
    style_value_converter();

    odf_style_family to_style_family(const pstring& val) const;
};

/**
 * Context that handles <office:automatic-styles> scope.
 */
class automatic_styles_context : public xml_context_base
{
public:
    automatic_styles_context(
        session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles, spreadsheet::iface::import_factory* factory);

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);
    virtual void start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    spreadsheet::iface::import_factory* mp_factory;
    odf_styles_map_type& m_styles;

    style_value_converter m_converter;

    unique_ptr<odf_style> m_current_style;
};

}

#endif
