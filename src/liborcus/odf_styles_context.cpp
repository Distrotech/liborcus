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

#include "odf_styles_context.hpp"
#include "odf_namespace_types.hpp"
#include "odf_token_constants.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

odf_style_family to_style_family(const pstring& val)
{
    static const char* p_graphic      = "graphic";
    static const char* p_paragraph    = "paragraph";
    static const char* p_table        = "table";
    static const char* p_table_column = "table-column";
    static const char* p_table_row    = "table-row";
    static const char* p_text         = "text";

    static size_t n_graphic      = strlen(p_graphic);
    static size_t n_paragraph    = strlen(p_paragraph);
    static size_t n_table        = strlen(p_table);
    static size_t n_table_column = strlen(p_table_column);
    static size_t n_table_row    = strlen(p_table_row);
    static size_t n_text         = strlen(p_text);

    if (val.size() < 4)
        return style_family_unknown;

    const char* p = val.get();
    const char* p_end = p + val.size();

    if (*p == 'g')
    {
        // The only choice is 'graphic'.
        if (val.size() != n_graphic)
            return style_family_unknown;

        return strncmp(p, p_graphic, n_graphic) ? style_family_unknown : style_family_graphic;
    }

    if (*p == 'p')
    {
        // The only choice is 'paragraph'.
        if (val.size() != n_paragraph)
            return style_family_unknown;

        return strncmp(p, p_paragraph, n_paragraph) ? style_family_unknown : style_family_paragraph;
    }

    if (*p == 't')
    {
        ++p;
        switch (*p)
        {
            case 'a':
            {
                // 'table', 'table-column' or 'table-row'.
                if (strncmp(p, p_table+1, n_table-1))
                    // Text doesn't begin with 'table'.
                    return style_family_unknown;

                p += 4; // Skip the 'able'.
                if (p == p_end)
                    return style_family_table;

                if (*p != '-')
                    // Text doesn't begin with 'table-'.
                    return style_family_unknown;

                ++p; // Skip the '-'.

                switch (*p)
                {
                    case 'c':
                    {
                        // The only choice is 'table-column'.
                        if (val.size() != n_table_column)
                            return style_family_unknown;

                        const char* p2 = p_table_column + 6;
                        return strncmp(p, p2, n_table_column-6) ? style_family_unknown : style_family_table_column;
                    }
                    case 'r':
                    {
                        // The only choice is 'table-row'.
                        if (val.size() != n_table_row)
                            return style_family_unknown;

                        const char* p2 = p_table_row + 6;
                        return strncmp(p, p2, n_table_row-6) ? style_family_unknown : style_family_table_row;
                    }
                    default:
                        ;
                }
            }
            break;
            case 'e':
                // The only choice is 'text'.
                if (val.size() != n_text)
                    return style_family_unknown;

                return strncmp(p, p_text+1, n_text-1) ? style_family_unknown : style_family_text;
            break;
            default:
                ;
        }
    }
    return style_family_unknown;
}

class style_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    pstring m_name;
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_name = attr.value;
                break;
                case XML_family:
                    // TODO: parse style familiy string into an enum.
                break;
            }
        }
    }

    const pstring& get_name() const { return m_name; }
};

class col_prop_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_column_width:
                    cout << "column width: " << attr.value << endl;
                break;
            }
        }
    }
};

class row_prop_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_row_height:
                    cout << "row height: " << attr.value << endl;
                break;
            }
        }
    }
};

}

automatic_styles_context::automatic_styles_context(session_context& session_cxt, const tokens& tk) :
    xml_context_base(session_cxt, tk),
    m_current_style_family(style_family_unknown)
{
}

bool automatic_styles_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* automatic_styles_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void automatic_styles_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void automatic_styles_context::start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_odf_office)
    {
        switch (name)
        {
            case XML_automatic_styles:
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            break;
            default:
                warn_unhandled();
        }
    }
    else if (ns == NS_odf_style)
    {
        switch (name)
        {
            case XML_style:
            {
                xml_element_expected(parent, NS_odf_office, XML_automatic_styles);
                style_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                m_current_style_name = func.get_name();
            }
            break;
            case XML_table_column_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                col_prop_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_table_row_properties:
            {
                xml_element_expected(parent, NS_odf_style, XML_style);
                row_prop_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_table_properties:
                xml_element_expected(parent, NS_odf_style, XML_style);
            break;
            case XML_paragraph_properties:
                xml_element_expected(parent, NS_odf_style, XML_style);
            break;
            case XML_text_properties:
                xml_element_expected(parent, NS_odf_style, XML_style);
            break;
            default:
                warn_unhandled();
        }
    }
    else
        warn_unhandled();
}

bool automatic_styles_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void automatic_styles_context::characters(const pstring& str)
{
}

}
