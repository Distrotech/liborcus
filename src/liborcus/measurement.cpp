/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "orcus/measurement.hpp"
#include "orcus/pstring.hpp"
#include "orcus/exception.hpp"
#include "orcus/parser_global.hpp"

#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>
#include <orcus/global.hpp>

using namespace std;

namespace orcus {

double to_double(const char* p, const char* p_end, const char** p_parse_ended)
{
    double val = parse_numeric(p, p_end-p);
    if (p_parse_ended)
        *p_parse_ended = p;

    return val;
}

double to_double(const pstring& s)
{
    const char* p = s.get();
    const char* p_end = p + s.size();
    return to_double(p, p_end, nullptr);
}

long to_long(const char* p, const char* p_end, const char** p_parse_ended)
{
    size_t n = p_end - p;
    long val = parse_integer(p, n);
    if (p_parse_ended)
        *p_parse_ended = p;

    return val;
}

long to_long(const pstring& s)
{
    const char* p = s.get();
    const char* p_end = p + s.size();
    return to_long(p, p_end, nullptr);
}

bool to_bool(const pstring& s)
{
    size_t n = s.size();
    if (n == 1)
        // Any single char other than '0' is true.
        return *s.get() != '0';

    if (n == 4)
    {
        // Check against 'true'.
        const char* p = s.get();
        if (*p++ != 't' || *p++ != 'r' || *p++ != 'u' || *p != 'e')
            return false;

        return true;
    }

    return false;
}

namespace {

typedef mdds::sorted_string_map<length_unit_t> length_map;

length_map::entry length_map_entries[] =
{
    {MDDS_ASCII("cm"), length_unit_t::centimeter},
    {MDDS_ASCII("in"), length_unit_t::inch},
    {MDDS_ASCII("mm"), length_unit_t::millimeter},
    {MDDS_ASCII("pt"), length_unit_t::point},
    {MDDS_ASCII("px"), length_unit_t::pixel}
};

}

length_t to_length(const pstring& str)
{
    length_t ret;
    if (str.empty())
        return ret;

    const char* p = str.get();
    const char* p_start = p;
    const char* p_end = p_start + str.size();
    ret.value = parse_numeric(p, p_end-p);

    static const length_map units(length_map_entries, ORCUS_N_ELEMENTS(length_map_entries), length_unit_t::unknown);
    pstring tail(p, p_end-p);
    ret.unit = units.find(tail.get(), tail.size());

    return ret;
}

namespace {

double convert_inch(double value, length_unit_t unit_to)
{
    switch (unit_to)
    {
        case length_unit_t::twip:
            // inches to twips : 1 twip = 1/1440 inches
            return value * 1440.0;
        default:
            ;
    }

    throw general_error("convert_inch: unsupported unit of measurement.");
}

double convert_point(double value, length_unit_t unit_to)
{
    // 72 points = 1 inch
    value /= 72.0;
    return convert_inch(value, unit_to);
}

double convert_centimeter(double value, length_unit_t unit_to)
{
    switch (unit_to)
    {
        case length_unit_t::twip:
            // centimeters to twips : 2.54 cm = 1 inch = 1440 twips
            return value / 2.54 * 1440.0;
        default:
            ;
    }

    throw general_error("convert_centimeter: unsupported unit of measurement.");
}

double convert_twip(double value, length_unit_t unit_to)
{
    switch (unit_to)
    {
        case length_unit_t::inch:
            // twips to inches : 1 twip = 1/1440 inches
            return value / 1440.0;
        default:
            ;
    }
    throw general_error("convert_twip: unsupported unit of measurement.");
}

/**
 * Since Excel's column width is based on the maximum digit width of font
 * used as the "Normal" style font, it's impossible to convert it accurately
 * without the font information.
 */
double convert_xlsx_column_digit(double value, length_unit_t unit_to)
{
    // Convert to centimeters first. Here, we'll just assume that a single
    // digit always equals 1.9 millimeters. TODO: find a better way to convert
    // this.
    value *= 0.19;
    return convert_centimeter(value, unit_to);
}

}

double convert(double value, length_unit_t unit_from, length_unit_t unit_to)
{
    switch (unit_from)
    {
        case length_unit_t::point:
            return convert_point(value, unit_to);
        case length_unit_t::inch:
            return convert_inch(value, unit_to);
        case length_unit_t::centimeter:
            return convert_centimeter(value, unit_to);
        case length_unit_t::twip:
            return convert_twip(value, unit_to);
        case length_unit_t::xlsx_column_digit:
            return convert_xlsx_column_digit(value, unit_to);
        default:
            ;
    }
    throw general_error("convert: unsupported unit of measurement.");
}

}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
