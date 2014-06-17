/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ORCUS_ODSCONTEXT_HPP__
#define __ORCUS_ODSCONTEXT_HPP__

#include "xml_context_base.hpp"
#include "odf_para_context.hpp"
#include "odf_styles.hpp"
#include "orcus/spreadsheet/types.hpp"

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

class ods_content_xml_context : public xml_context_base
{
    typedef boost::unordered_map<pstring, size_t, pstring::hash> name2id_type;

public:
    struct row_attr
    {
        long number_rows_repeated;
        row_attr();
    };

    enum cell_value_type { vt_unknown, vt_float, vt_string, vt_date };

    struct cell_attr
    {
        long number_columns_repeated;
        cell_value_type type;
        double value;
        pstring date_value;
        pstring style_name;

        pstring formula;
        spreadsheet::formula_grammar_t formula_grammar;

        cell_attr();
    };

    ods_content_xml_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~ods_content_xml_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void start_null_date(const xml_attrs_t& attrs);

    void start_table(const xml_attrs_t& attrs);
    void end_table();

    void start_column(const xml_attrs_t& attrs);
    void end_column();

    void start_row(const xml_attrs_t& attrs);
    void end_row();

    void start_cell(const xml_attrs_t& attrs);
    void end_cell();

    void push_cell_value();

    void end_spreadsheet();

private:
    spreadsheet::iface::import_factory* mp_factory;
    std::vector<spreadsheet::iface::import_sheet*> m_tables;

    boost::scoped_ptr<xml_context_base> mp_child;

    row_attr    m_row_attr;
    cell_attr   m_cell_attr; /// attributes of current cell.

    int m_row;
    int m_col;
    size_t m_para_index;
    bool m_has_content;

    odf_styles_map_type m_styles; /// map storing all automatic styles by their names.
    name2id_type m_cell_format_map; /// map of style names to cell format (xf) IDs.

    text_para_context m_child_para;
};

}

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
