/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ORCUS_GNUMERIC_SHEET_CONTEXT_HPP
#define INCLUDED_ORCUS_GNUMERIC_SHEET_CONTEXT_HPP

#include "xml_context_base.hpp"
#include "orcus/spreadsheet/types.hpp"

#include "orcus/string_pool.hpp"

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;
class import_auto_filter;

}}

struct gnumeric_color
{
    spreadsheet::color_elem_t red;
    spreadsheet::color_elem_t green;
    spreadsheet::color_elem_t blue;

    gnumeric_color():
        red(0),
        green(0),
        blue(0) {}
};

struct gnumeric_style_region
{
    spreadsheet::row_t start_row;
    spreadsheet::row_t end_row;
    spreadsheet::col_t start_col;
    spreadsheet::col_t end_col;

    size_t xf_id;
    bool contains_conditional_format;

    gnumeric_style_region():
        start_row(0),
        end_row(0),
        start_col(0),
        end_col(0),
        xf_id(0),
        contains_conditional_format(false) {}
};

class gnumeric_sheet_context : public xml_context_base
{
public:
    gnumeric_sheet_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~gnumeric_sheet_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void start_style_region(const xml_attrs_t& attrs);
    void start_style(const xml_attrs_t& attrs);
    void start_font(const xml_attrs_t& attrs);
    void start_col(const xml_attrs_t& attrs);
    void start_row(const xml_attrs_t& attrs);
    void start_condition(const xml_attrs_t& attrs);

    void end_table();
    void end_style(bool conditional_format);
    void end_font();
    void end_style_region();
    void end_condition();
    void end_expression();

private:
    spreadsheet::iface::import_factory* mp_factory;

    spreadsheet::iface::import_sheet* mp_sheet;
    spreadsheet::iface::import_auto_filter* mp_auto_filter;

    std::unique_ptr<xml_context_base> mp_child;
    std::unique_ptr<gnumeric_style_region> mp_region_data;

    gnumeric_color front_color;

    string_pool m_pool;

    /**
     * Used for temporary storage of characters
     */
    pstring chars;
};

} // namespace orcus

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
