/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "odf_helper.hpp"
#include "string_helper.hpp"
#include <orcus/spreadsheet/types.hpp>
#include <orcus/measurement.hpp>
#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>
#include <orcus/global.hpp>
#include <orcus/spreadsheet/styles.hpp>

namespace orcus {

namespace {

typedef mdds::sorted_string_map<spreadsheet::border_style_t> odf_border_style_map;

odf_border_style_map::entry odf_border_style_entries[] =
{
    { MDDS_ASCII("dash-dot"), spreadsheet::border_style_t::dash_dot},
    { MDDS_ASCII("dash-dot-dot"), spreadsheet::border_style_t::dash_dot_dot},
    { MDDS_ASCII("dashed"), spreadsheet::border_style_t::dashed},
    { MDDS_ASCII("dotted"), spreadsheet::border_style_t::dotted},
    { MDDS_ASCII("double-thin"), spreadsheet::border_style_t::double_thin},
    { MDDS_ASCII("fine-dashed"), spreadsheet::border_style_t::fine_dashed},
    { MDDS_ASCII("none"), spreadsheet::border_style_t::none},
    { MDDS_ASCII("solid"), spreadsheet::border_style_t::solid},
    { MDDS_ASCII("unknown"), spreadsheet::border_style_t::unknown}
};

typedef mdds::sorted_string_map<spreadsheet::underline_width_t> odf_underline_width_map;

odf_underline_width_map::entry odf_underline_width_entries[] =
{
    { MDDS_ASCII("bold"), spreadsheet::underline_width_t::bold},
    { MDDS_ASCII("medium"), spreadsheet::underline_width_t::medium},
    { MDDS_ASCII("none"), spreadsheet::underline_width_t::none},
    { MDDS_ASCII("normal"), spreadsheet::underline_width_t::normal},
    { MDDS_ASCII("percent"), spreadsheet::underline_width_t::percent},
    { MDDS_ASCII("positiveInteger"), spreadsheet::underline_width_t::positive_integer},
    { MDDS_ASCII("positiveLength"), spreadsheet::underline_width_t::positive_length},
    { MDDS_ASCII("thick"), spreadsheet::underline_width_t::thick},
    { MDDS_ASCII("thin"), spreadsheet::underline_width_t::thin},
};

typedef mdds::sorted_string_map<spreadsheet::underline_t> odf_underline_style_map;

odf_underline_style_map::entry odf_underline_style_entries[] =
{
    { MDDS_ASCII("dash"), spreadsheet::underline_t::dash},
    { MDDS_ASCII("dot-dash"), spreadsheet::underline_t::dot_dash},
    { MDDS_ASCII("dot-dot-dot-dash"), spreadsheet::underline_t::dot_dot_dot_dash},
    { MDDS_ASCII("dotted"), spreadsheet::underline_t::dotted},
    { MDDS_ASCII("long-dash"), spreadsheet::underline_t::long_dash},
    { MDDS_ASCII("none"), spreadsheet::underline_t::none},
    { MDDS_ASCII("solid"), spreadsheet::underline_t::solid},
    { MDDS_ASCII("wave"), spreadsheet::underline_t::wave}
};

bool is_valid_hex_digit(const char& character, orcus::spreadsheet::color_elem_t& val)
{
    if ('0' <= character && character <= '9')
    {
        val += character - '0';
        return true;
    }

    if ('A' <= character && character <= 'F')
    {
        val += character - 'A' + 10;
        return true;
    }

    if ('a' <= character && character <= 'f')
    {
        val += character - 'a' + 10;
        return true;
    }

    return false;
}

// converts two characters starting at index to a color value
bool convert_color_digits(const pstring& value, orcus::spreadsheet::color_elem_t& color_val, size_t index)
{
    const char& high_val = value[index];
    color_val = 0;
    if (!is_valid_hex_digit(high_val, color_val))
        return false;
    color_val *= 16;
    const char& low_val = value[++index];
    return is_valid_hex_digit(low_val, color_val);
}

}

bool odf_helper::convert_fo_color(const pstring& value, orcus::spreadsheet::color_elem_t& red,
        orcus::spreadsheet::color_elem_t& green, orcus::spreadsheet::color_elem_t& blue)
{
    // first character needs to be '#'
    if (value.size() != 7)
        return false;

    if (value[0] != '#')
        return false;

    if (!convert_color_digits(value, red, 1))
        return false;

    if (!convert_color_digits(value, green, 3))
        return false;

    return convert_color_digits(value, blue, 5);
}

orcus::odf_helper::odf_border_details odf_helper::extract_border_details(const orcus::pstring &value)
{
    orcus::odf_helper::odf_border_details border_details;

    std::vector<pstring> detail = orcus::string_helper::split_string(value,' ');

    for (auto& sub_detail : detail)
    {
        if (sub_detail[0] == '#')
            convert_fo_color(sub_detail, border_details.red, border_details.green, border_details.blue);
        else if (sub_detail[0] >= '0' && sub_detail[0] <='9')
            border_details.border_width = orcus::to_length(sub_detail);
        else    //  This has to be a style
        {
            odf_border_style_map border_style_map(odf_border_style_entries, ORCUS_N_ELEMENTS(odf_border_style_entries), spreadsheet::border_style_t::none);
            border_details.border_style = border_style_map.find(sub_detail.get(), sub_detail.size());
        }

    }
    return border_details;
}

orcus::spreadsheet::underline_width_t odf_helper::extract_underline_width(const orcus::pstring& value)
{
    orcus::spreadsheet::underline_width_t underline_width;

    odf_underline_width_map underline_width_map(odf_underline_width_entries, ORCUS_N_ELEMENTS(odf_underline_width_entries), spreadsheet::underline_width_t::none);
    underline_width = underline_width_map.find(value.get(), value.size());

    return underline_width;
}

orcus::spreadsheet::underline_t odf_helper::extract_underline_style(const orcus::pstring& value)
{
    spreadsheet::underline_t underline_style;

    odf_underline_style_map underline_style_map(odf_underline_style_entries, ORCUS_N_ELEMENTS(odf_underline_style_entries), spreadsheet::underline_t::none);
    underline_style = underline_style_map.find(value.get(), value.size());

    return underline_style;
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
