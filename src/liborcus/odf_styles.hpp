/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_ODF_STYLES_HPP
#define INCLUDED_ORCUS_ODF_STYLES_HPP

#include "orcus/pstring.hpp"
#include "orcus/measurement.hpp"

#include <map>
#include <memory>

namespace orcus {

enum odf_style_family
{
    style_family_unknown = 0,
    style_family_table_column,
    style_family_table_row,
    style_family_table_cell,
    style_family_table,
    style_family_graphic,
    style_family_paragraph,
    style_family_text
};

/**
 * Each instance of this class represents a single <style:style> entry.
 */
struct odf_style
{
    struct column
    {
        length_t width;
    };

    struct row
    {
        length_t height;
    };

    struct cell
    {
        size_t font;
        size_t fill;
        size_t border;
        size_t protection;

        size_t xf;
        bool automatic_style;

        cell() : font(0), fill(0), border(0), protection(0),
                xf(0), automatic_style(false) {}
    };

    struct table
    {
    };

    struct graphic
    {
    };

    struct paragraph
    {
    };

    struct text
    {
        size_t font;
    };

    pstring name;
    odf_style_family family;
    pstring parent_name;

    union {
        column* column_data;
        row* row_data;
        table* table_data;
        cell* cell_data;
        graphic* graphic_data;
        paragraph* paragraph_data;
        text* text_data;
    };

    odf_style(const odf_style&) = delete;
    odf_style& operator=(const odf_style&) = delete;

    odf_style();
    odf_style(const pstring& _name, odf_style_family _family, const pstring& parent);

    ~odf_style();
};

struct number_formatting_style
{
    size_t number_formatting;

    pstring name;
    std::string number_formatting_code;
    bool is_volatile;
    pstring character_stream;

    number_formatting_style():
        is_volatile(false)
    {}

    number_formatting_style(const pstring& style_name, const bool volatile_style);
};

typedef std::map<pstring, std::unique_ptr<odf_style>> odf_styles_map_type;

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
