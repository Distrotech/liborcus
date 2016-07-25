/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_revision_context.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_token_constants.hpp"
#include "session_context.hpp"
#include "xml_context_global.hpp"

#include "orcus/measurement.hpp"
#include "orcus/string_pool.hpp"
#include "orcus/global.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

class headers_attr_parser : public unary_function<xml_token_attr_t, void>
{
    pstring m_guid;
    long m_highest_revid;
    long m_version;
    bool m_disk_revisions;

public:
    headers_attr_parser() : m_highest_revid(-1), m_version(-1), m_disk_revisions(false) {}

    pstring get_last_guid() const { return m_guid; }
    long get_highest_revid() const { return m_highest_revid; }
    long get_version() const { return m_version; }
    bool is_disk_revisions() const { return m_disk_revisions; }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            // All attributes are in the xlsx namespace.
            return;

        switch (attr.name)
        {
            case XML_guid:
                // guid should never be transient as it's not supposed to contain any encoded characters.
                // TODO : We should convert guid to a numeric value here.
                m_guid = attr.value;
            break;
            case XML_diskRevisions:
                m_disk_revisions = to_long(attr.value) != 0;
            break;
            case XML_revisionId:
                m_highest_revid = to_long(attr.value);
            break;
            case XML_version:
                m_version = to_long(attr.value);
            break;
            default:
                ;
        }
    }
};

class header_attr_parser : public unary_function<xml_token_attr_t, void>
{
    string_pool* m_pool;

    pstring m_guid;
    pstring m_username;
    pstring m_rid;

    date_time_t m_date_time;
    long m_next_sheet_id;
    long m_min_revid;
    long m_max_revid;

public:
    header_attr_parser(string_pool& pool) :
        m_pool(&pool), m_next_sheet_id(-1), m_min_revid(-1), m_max_revid(-1) {}

    pstring get_guid() const { return m_guid; }
    pstring get_username() const { return m_username; }
    pstring get_rid() const { return m_rid; }
    date_time_t get_date_time() const { return m_date_time; }
    long get_next_sheet_id() const { return m_next_sheet_id; }
    long get_min_revid() const { return m_min_revid; }
    long get_max_revid() const { return m_max_revid; }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_ooxml_xlsx)
        {
            switch (attr.name)
            {
                case XML_guid:
                    m_guid = attr.value;
                break;
                case XML_dateTime:
                    m_date_time = to_date_time(attr.value);
                break;
                case XML_maxSheetId:
                    m_next_sheet_id = to_long(attr.value);
                break;
                case XML_userName:
                    m_username = attr.value;
                    if (attr.transient)
                        m_username = m_pool->intern(m_username).first;
                break;
                case XML_minRId:
                    m_min_revid = to_long(attr.value);
                break;
                case XML_maxRId:
                    m_max_revid = to_long(attr.value);
                break;
                default:
                    ;
            }
        }
        else if (attr.ns == NS_ooxml_r && attr.name == XML_id)
        {
            // Pick up a rel id here.
            if (attr.transient)
                // Rel ID's should never be transient.
                return;

            m_rid = attr.value;
        }
    }
};

}

xlsx_revheaders_context::xlsx_revheaders_context(session_context& session_cxt, const tokens& tokens) :
    xml_context_base(session_cxt, tokens) {}

xlsx_revheaders_context::~xlsx_revheaders_context() {}

bool xlsx_revheaders_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_revheaders_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return nullptr;
}

void xlsx_revheaders_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_revheaders_context::start_element(xmlns_id_t ns, xml_token_t name, const vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_headers:
            {
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
                headers_attr_parser func;
                func = for_each(attrs.begin(), attrs.end(), func);
                cout << "* last guid: " << func.get_last_guid() << endl;
                cout << "* highest revision ID: " << func.get_highest_revid() << endl;
                cout << "* version: " << func.get_version() << endl;
                cout << "* disk revisions: " << func.is_disk_revisions() << endl;
            }
            break;
            case XML_header:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_headers);
                header_attr_parser func(get_session_context().m_string_pool);
                func = for_each(attrs.begin(), attrs.end(), func);
                cout << "* revision header (guid:" << func.get_guid() << ")" << endl;
                cout << "  - timestamp: " << func.get_date_time().to_string() << endl;
                cout << "  - user name: " << func.get_username() << endl;

                if (func.get_min_revid() != -1 && func.get_max_revid() != -1)
                    cout << "  - revision range: " << func.get_min_revid() << "-" << func.get_max_revid() << endl;

                long next_sheet = func.get_next_sheet_id();
                if (next_sheet != -1)
                    cout << "  - next available sheet: " << (next_sheet - 1) << endl;

                cout << "  - revision log rid: " << func.get_rid() << endl;
                // TODO : Intern the rid here when passing it to the revision log stream.
            }
            break;
            case XML_sheetIdMap:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_header);
                m_cur_sheet_ids.clear();
                long n = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                if (n > 0)
                    m_cur_sheet_ids.reserve(n);
            }
            break;
            case XML_sheetId:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_sheetIdMap);
                long val = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_val);
                if (val > 0)
                    m_cur_sheet_ids.push_back(val-1); // convert from 1-based to 0-based.
            }
            break;
            default:
                warn_unhandled();
        }
    }
}

bool xlsx_revheaders_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_sheetIdMap:
            {
                cout << "  - sheet indices: ";
                for (size_t i = 0; i < m_cur_sheet_ids.size(); ++i)
                    cout << m_cur_sheet_ids[i] << " ";
                cout << endl;
            }
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void xlsx_revheaders_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

namespace {

class rcc_attr_parser : public unary_function<xml_token_attr_t, void>
{
    long m_revision_id;
    long m_sheet_id;

public:
    rcc_attr_parser() : m_revision_id(-1), m_sheet_id(-1) {}

    long get_revision_id() const { return m_revision_id; }
    long get_sheet_id() const { return m_sheet_id; }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_rId:
                // revision ID
                m_revision_id = to_long(attr.value);
            break;
            case XML_sId:
                // sheet ID
                m_sheet_id = to_long(attr.value);
            break;
            default:
                ;
        }
    }
};

class rrc_attr_parser : public unary_function<xml_token_attr_t, void>
{
    long m_revision_id;
    long m_sheet_id;

    pstring m_ref;
    xlsx_rev_row_column_action_t m_action_type;

    bool m_end_of_list;

public:
    rrc_attr_parser() : m_revision_id(-1), m_sheet_id(-1),
        m_action_type(xlsx_rev_rca_unknown), m_end_of_list(false) {}

    long get_revision_id() const { return m_revision_id; }
    long get_sheet_id() const { return m_sheet_id; }
    bool is_end_of_list() const { return m_end_of_list; }
    pstring get_ref() const { return m_ref; }
    xlsx_rev_row_column_action_t get_action_type() const { return m_action_type; }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_rId:
                // revision ID
                m_revision_id = to_long(attr.value);
            break;
            case XML_sId:
                // sheet ID
                m_sheet_id = to_long(attr.value);
            break;
            case XML_eol:
                m_end_of_list = to_long(attr.value) > 0;
            break;
            case XML_ref:
                if (!attr.transient)
                    m_ref = attr.value;
            break;
            case XML_action:
                m_action_type = to_xlsx_rev_row_column_action_type(attr.value);
            break;
            default:
                ;
        }
    }
};

class cell_data_attr_parser : public unary_function<xml_token_attr_t, void>
{
    pstring m_ref;
    xlsx_cell_t m_type;

public:
    cell_data_attr_parser() : m_type(xlsx_ct_numeric) {}

    pstring get_ref() const { return m_ref; }
    xlsx_cell_t get_cell_type() const { return m_type; }

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_r:
                if (!attr.transient)
                    m_ref = attr.value;
            break;
            case XML_t:
                m_type = to_xlsx_cell_type(attr.value);
            default:
                ;
        }
    }
};

}

xlsx_revlog_context::xlsx_revlog_context(session_context& session_cxt, const tokens& tokens) :
    xml_context_base(session_cxt, tokens),
    m_cur_value(std::numeric_limits<double>::quiet_NaN()),
    m_cur_cell_type(xlsx_ct_unknown), m_cur_formula(false)
{
}

xlsx_revlog_context::~xlsx_revlog_context() {}

bool xlsx_revlog_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_revlog_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return nullptr;
}

void xlsx_revlog_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_revlog_context::start_element(xmlns_id_t ns, xml_token_t name, const vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_revisions:
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            break;
            case XML_raf:
                // revision auto format
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rcc:
            {
                // revision cell change
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
                rcc_attr_parser func;
                func = for_each(attrs.begin(), attrs.end(), func);
                cout << "* revision id: " << func.get_revision_id() << "  type: cell change" << endl;
                cout << "  - sheet index: " << func.get_sheet_id() << endl;
                m_cur_cell_type = xlsx_ct_unknown;
            }
            break;
            case XML_rcft:
                // revision merge conflict
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rcmt:
                // revision cell comment
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rcv:
                // revision custom view
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rdn:
                // revision defined name
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rfmt:
                // revision format
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_ris:
                // revision insert sheet
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rm:
                // revision cell move
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rqt:
                // revision query table
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_rrc:
            {
                // revision row column insert delete
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
                rrc_attr_parser func;
                func = for_each(attrs.begin(), attrs.end(), func);
                cout << "* revision id: " << func.get_revision_id() << "  type: row column insert delete" << endl;
                cout << "  - sheet index: " << func.get_sheet_id() << endl;
                cout << "  - action type: " << to_string(func.get_action_type()) << endl;
                cout << "  - range: " << func.get_ref() << endl;
                cout << "  - end of list: " << (func.is_end_of_list() ? "true":"false") << endl;
            }
            break;
            case XML_rsnm:
                // revision sheet name
                xml_element_expected(parent, NS_ooxml_xlsx, XML_revisions);
            break;
            case XML_oc:
                // old cell data
                xml_element_expected(parent, NS_ooxml_xlsx, XML_rcc);
            break;
            case XML_nc:
            {
                // new cell data
                xml_element_expected(parent, NS_ooxml_xlsx, XML_rcc);
                cell_data_attr_parser func;
                func = for_each(attrs.begin(), attrs.end(), func);
                m_cur_cell_type = func.get_cell_type();

                m_cur_formula = false;
                m_cur_value = 0.0;
                m_cur_string.clear();

                cout << "  - new cell position: " << func.get_ref() << endl;
                cout << "  - new cell type: " << to_string(m_cur_cell_type) << endl;
            }
            break;
            case XML_f:
            {
                xml_elem_stack_t elems;
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_oc));
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_nc));
                xml_element_expected(parent, elems);
            }
            break;
            case XML_v:
            {
                xml_elem_stack_t elems;
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_oc));
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_nc));
                xml_element_expected(parent, elems);
            }
            break;
            case XML_is:
            {
                xml_elem_stack_t elems;
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_oc));
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_nc));
                xml_element_expected(parent, elems);
            }
            break;
            case XML_t:
            {
                xml_elem_stack_t elems;
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_is));
                elems.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_r));
                xml_element_expected(parent, elems);
            }
            break;
            default:
                warn_unhandled();
        }
    }
}

bool xlsx_revlog_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_nc:
            {
                cout << "  - new cell value: ";
                switch (m_cur_cell_type)
                {
                    case xlsx_ct_boolean:
                        if (m_cur_value != 0.0)
                            cout << "true";
                        else
                            cout << "false";
                    break;
                    case xlsx_ct_numeric:
                        if (m_cur_formula)
                            cout << m_cur_string;
                        else
                            cout << m_cur_value;
                    break;
                    case xlsx_ct_inline_string:
                        cout << m_cur_string;
                    break;
                    default:
                        ;
                }
                cout << endl;
            }
            break;
            default:
                ;
        }
    }
    return pop_stack(ns, name);
}

void xlsx_revlog_context::characters(const pstring& str, bool transient)
{
    const xml_token_pair_t& elem = get_current_element();
    if (elem.first == NS_ooxml_xlsx)
    {
        switch (elem.second)
        {
            case XML_v:
                m_cur_value = to_double(str);
            break;
            case XML_f:
                m_cur_formula = true;
                // fall through to get the string.
            case XML_t:
                m_cur_string = str;
                if (transient)
                    m_cur_string = get_session_context().m_string_pool.intern(m_cur_string).first;
            break;
            default:
                ;
        }
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
