/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_number_formatting_context.hpp"
#include "odf_namespace_types.hpp"
#include "odf_token_constants.hpp"
#include "odf_helper.hpp"
#include "orcus/measurement.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include <orcus/spreadsheet/styles.hpp>

#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

namespace orcus {

namespace {

class number_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_country_code;
    pstring m_style_name;
    pstring m_language;
    bool m_volatile;

public:

    number_style_attr_parser():
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
        {
            switch(attr.name)
            {
                case XML_country:
                    m_country_code = attr.value;
                break;
                case XML_language:
                    m_language = attr.value;
                break;
                default:
                    ;
            }
        }
        else if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    pstring get_country_code() const { return m_country_code;}
    bool is_volatile() const { return m_volatile;}
    pstring get_language() const { return m_language;}
};

class number_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    size_t m_decimal_places;
    size_t m_min_int_digits;
    bool m_grouping;
    bool m_has_decimal_places;

public:

    number_attr_parser() :
        m_grouping(false),
        m_has_decimal_places(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
        {
            switch (attr.name)
            {
                case XML_decimal_places:
                {
                    m_decimal_places = to_long(attr.value);
                    if (m_decimal_places > 0)
                        m_has_decimal_places = true;
                }
                break;
                case XML_grouping:
                    m_grouping = attr.value == "true";
                break;
                case XML_min_integer_digits:
                    m_min_int_digits = to_long(attr.value);
                break;
                default:
                    ;
            }
        }
    }

    size_t get_decimal_places() const { return m_decimal_places;}
    bool is_grouped() const { return m_grouping;}
    size_t get_min_int_digits() const { return m_min_int_digits;}
    bool has_decimal_places() const { return m_has_decimal_places;}
};

class scientific_number_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    size_t m_decimal_places;
    bool m_grouping;
    size_t m_min_exp_digits;
    size_t m_min_int_digits;

public:

    scientific_number_attr_parser() :
        m_grouping(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
        {
            switch(attr.name)
            {
                case XML_decimal_places:
                    m_decimal_places = to_long(attr.value);
                break;
                case XML_grouping:
                    m_grouping = attr.value == "true";
                break;
                case XML_min_exponent_digits:
                    m_min_exp_digits = to_long(attr.value);
                break;
                case XML_min_integer_digits:
                    m_min_int_digits = to_long(attr.value);
                break;
                default:
                    ;
            }
        }
    }

    size_t get_decimal_places() const { return m_decimal_places;}
    bool is_grouped() const { return m_grouping;}
    size_t get_min_exp_digits() const { return m_min_exp_digits;}
    size_t get_min_int_digits() const { return m_min_int_digits;}
};

class percentage_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    percentage_style_attr_parser() :
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};

class currency_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    currency_style_attr_parser() :
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};


class date_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    date_style_attr_parser():
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }
    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};

class day_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;

public:
    day_attr_parser():
        m_style_name(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
            if (attr.name == XML_style)
                m_style_name = attr.value == "true";
    }

    bool has_long() const { return m_style_name;}

};

class year_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;

public:
    year_attr_parser():
        m_style_name(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
            if (attr.name == XML_style)
                m_style_name = attr.value =="true";
    }

    bool has_long() const { return m_style_name;}

};

class month_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;
    bool m_textual;

public:
    month_attr_parser():
        m_textual(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
        {
            if (attr.name == XML_style)
                m_style_name = attr.value == "true";
            if (attr.name == XML_textual)
                m_textual = attr.value == "true";
        }
    }

    bool has_long() const { return m_style_name;}
    bool is_textual() const { return m_textual;}
};


class time_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    time_style_attr_parser():
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};

class hours_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;

public:
    hours_attr_parser():
        m_style_name()
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
            if (attr.name == XML_style)
                m_style_name = attr.value == "true";
    }

    bool has_long() const { return m_style_name;}
};

class minutes_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;

public:
    minutes_attr_parser():
        m_style_name(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
            if (attr.name == XML_style)
                m_style_name = attr.value == "true";
    }

    bool has_long() const { return m_style_name;}
};

class seconds_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    bool m_style_name;

public:
    seconds_attr_parser():
        m_style_name(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
            if (attr.name == XML_style)
                m_style_name = attr.value == "true";
    }

    bool has_long() const { return m_style_name;}
};


class fraction_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    size_t m_min_int_digits;
    size_t m_min_deno_digits;
    size_t m_min_num_digits;

public:

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_number)
        {
            switch(attr.name)
            {
                case XML_min_integer_digits:
                    m_min_int_digits = to_long(attr.value);
                break;
                case XML_min_numerator_digits:
                    m_min_num_digits = to_long(attr.value);
                break;
                case XML_min_denominator_digits:
                    m_min_deno_digits = to_long(attr.value);
                break;
                default:
                    ;
            }
        }
    }

    size_t get_min_int_digits() const { return m_min_int_digits;}
    size_t get_min_num_digits() const { return m_min_num_digits;}
    size_t get_min_deno_digits() const { return m_min_deno_digits;}
};

class boolean_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    boolean_style_attr_parser():
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};

class text_style_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_style_name;
    bool m_volatile;

public:
    text_style_attr_parser():
        m_volatile(false)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_style)
        {
            switch (attr.name)
            {
                case XML_name:
                    m_style_name = attr.value;
                break;
                case XML_volatile:
                    m_volatile = attr.value == "true";
                break;
                default:
                    ;
            }
        }
    }

    pstring get_style_name() const { return m_style_name;}
    bool is_volatile() const { return m_volatile;}
};

class text_properties_attr_parser : std::unary_function<xml_token_attr_t, void>
{
    pstring m_color;
    bool color_absent;

public:
    text_properties_attr_parser():
        color_absent(true)
    {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_odf_fo)
        {
            switch (attr.name)
            {
                case XML_color:
                {
                    if (attr.value == "#000000")
                        m_color = "BLACK";
                    if (attr.value == "#ff0000")
                        m_color = "RED";
                    if (attr.value == "#00ff00")
                        m_color = "GREEN";
                    if (attr.value == "#0000ff")
                        m_color = "BLUE";
                    if (attr.value == "#ffff00")
                        m_color = "YELLOW";
                    if (attr.value == "#00ffff")
                        m_color = "CYAN";
                    if (attr.value == "#ff00ff")
                        m_color = "MAGENTA";
                    if (attr.value == "#ffffff")
                        m_color = "WHITE";
                    else
                        color_absent = false;
                }
            }
        }
    }

    pstring get_color() const { return m_color;}
    bool has_color() const { return !color_absent;}
};

}

number_formatting_context::number_formatting_context(
    session_context& session_cxt, const tokens& tk, odf_styles_map_type& styles,
    spreadsheet::iface::import_styles* iface_styles, number_formatting_style* number_format_style) :
    xml_context_base(session_cxt, tk),
    mp_styles(iface_styles),
    m_styles(styles),
    m_current_style(number_format_style)
{}

bool number_formatting_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* number_formatting_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void number_formatting_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void number_formatting_context::start_element(xmlns_id_t ns, xml_token_t name, const std::vector<xml_token_attr_t>& attrs)
{
    if (ns == NS_odf_number)
    {
        switch(name)
        {
            case XML_number_style:
            {
                number_style_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                m_current_style->name = func.get_style_name();
                m_current_style->is_volatile = func.is_volatile();
            }
            break;
            case XML_number:
            {
                number_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                if (func.is_grouped())
                {
                    if (func.get_min_int_digits() < 4)
                    {
                        m_current_style->number_formatting_code += "#,";
                        for (size_t i = 0; i < 3 - func.get_min_int_digits(); i++)
                        {
                            m_current_style->number_formatting_code += "#";
                        }
                        for (size_t i = 0; i < func.get_min_int_digits(); i++)
                        {
                            m_current_style->number_formatting_code += "0";
                        }
                    }
                    else
                    {
                        std:: string temporary_code;
                        for(size_t i = 0; i < func.get_min_int_digits(); i++)
                        {
                            if (i % 3 == 0 && i != 0)
                                temporary_code += ",";
                            temporary_code += "0";
                        }
                        std::reverse(temporary_code.begin(), temporary_code.end());
                        m_current_style->number_formatting_code += temporary_code;
                    }
                }
                else
                {
                    if (func.get_min_int_digits() == 0)
                        m_current_style->number_formatting_code += "#";

                    for (size_t i = 0; i < func.get_min_int_digits(); i++)
                    {
                        m_current_style->number_formatting_code += "0";
                    }
                }
                if (func.has_decimal_places())
                {
                    m_current_style->number_formatting_code += ".";
                    for(size_t i = 0; i < func.get_decimal_places() ; i++)
                        m_current_style->number_formatting_code += "0";
                }
            }
            break;
            case XML_currency_style:
            {
                currency_style_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                m_current_style->name = func.get_style_name();
                m_current_style->is_volatile = func.is_volatile();
            }
            break;
            default:
                ;
        }
    }
    if (ns == NS_odf_style)
    {
        switch (name)
        {
            case XML_text_properties:
            {
                text_properties_attr_parser func;
                func = std::for_each(attrs.begin(), attrs.end(), func);
                if (func.has_color())
                    m_current_style->number_formatting_code = m_current_style->number_formatting_code + "[" + func.get_color() + "]";
            }
            break;
            default:
                ;
        }
    }
}

bool number_formatting_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_odf_number)
    {
        if (name == XML_number_style || name == XML_currency_style)
        {
            if (m_current_style->is_volatile)
            {
                m_current_style->number_formatting_code += ";";
            }
            else
            {
                mp_styles->set_number_format_code(m_current_style->number_formatting_code.c_str(),
                                                m_current_style->number_formatting_code.size());
                mp_styles->set_xf_number_format(mp_styles->commit_number_format());

                mp_styles->set_cell_style_name( m_current_style->name.get(), m_current_style->name.size());
                mp_styles->set_cell_style_xf(mp_styles->commit_cell_style_xf());
                mp_styles->commit_cell_style();
                return true;
            }
        }
        else if (name == XML_currency_symbol)
            m_current_style->number_formatting_code = m_current_style->number_formatting_code + "[$"
                    + m_current_style->character_stream + "]";

        else if (name == XML_text)
                m_current_style->number_formatting_code += m_current_style->character_stream;
        }
    return false;
}


void number_formatting_context::characters(const pstring& str, bool transient)
{
    if (str != "\n")
        m_current_style->character_stream = str;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
