/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_pivot_context.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_token_constants.hpp"
#include "xml_context_global.hpp"

#include "orcus/measurement.hpp"

#include <iostream>

using namespace std;

namespace orcus {

namespace {

class cache_def_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns == NS_ooxml_xlsx)
        {
            switch (attr.name)
            {
                case XML_refreshedBy:
                    cout << "refreshed by: " << attr.value << endl;
                break;
                case XML_recordCount:
                    cout << "record count: " << attr.value << endl;
                break;
                default:
                    ;
            }
        }
        else if (attr.ns == NS_ooxml_r)
        {
            switch (attr.name)
            {
                case XML_id:
                    // relation id for its cache record.
                    cout << "rid: " << attr.value << endl;
                break;
                default:
                    ;
            }
        }
    }
};

class cache_src_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_type:
                cout << "type: " << attr.value << endl;
            break;
            default:
                ;
        }
    }
};

class worksheet_src_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_ref:
                cout << "ref: " << attr.value << endl;
            break;
            case XML_sheet:
                cout << "sheet: " << attr.value << endl;
            break;
            default:
                ;
        }
    }
};

class cache_field_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_name:
                cout << "* name: " << attr.value << endl;
            break;
            case XML_numFmtId:
                cout << "  number format id: " << attr.value << endl;
            break;
            default:
                ;
        }
    }
};

class shared_items_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        bool b = false;
        long v = 0;
        switch (attr.name)
        {
            case XML_count:
                cout << "  count: " << attr.value << endl;
            break;
            case XML_containsSemiMixedTypes:
                b = to_bool(attr.value);
                cout << "  contains semi-mixed types: " << b << endl;
            break;
            case XML_containsString:
                b = to_bool(attr.value);
                cout << "  contains string: " << b << endl;
            break;
            case XML_containsNumber:
                b = to_bool(attr.value);
                cout << "  contains number: " << b << endl;
            break;
            case XML_containsInteger:
                b = to_bool(attr.value);
                cout << "  contains integer: " << b << endl;
            break;
            case XML_minValue:
                v = to_long(attr.value);
                cout << "  min value: " << v << endl;
            break;
            case XML_maxValue:
                v = to_long(attr.value);
                cout << "  max value: " << v << endl;
            break;
            default:
                ;
        }
    }
};

class s_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_v:
                cout << "  * v: " << attr.value << endl;
            break;
            default:
                ;
        }
    }
};

}

xlsx_pivot_cache_def_context::xlsx_pivot_cache_def_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_cache_def_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_pivot_cache_def_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_pivot_cache_def_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_pivot_cache_def_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns != NS_ooxml_xlsx)
        return;

    switch (name)
    {
        case XML_pivotCacheDefinition:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            cache_def_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_cacheSource:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotCacheDefinition);
            cache_src_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_worksheetSource:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cacheSource);
            worksheet_src_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_cacheFields:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotCacheDefinition);
            single_long_attr_getter func(NS_ooxml_xlsx, XML_count);
            long field_count = for_each(attrs.begin(), attrs.end(), func).get_value();
            cout << "field count: " << field_count << endl;
        }
        break;
        case XML_cacheField:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cacheFields);
            cache_field_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_sharedItems:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cacheField);
            shared_items_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        case XML_s:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_sharedItems);
            s_attr_parser func;
            for_each(attrs.begin(), attrs.end(), func);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_pivot_cache_def_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_cache_def_context::characters(const pstring& str, bool transient)
{
}

xlsx_pivot_cache_rec_context::xlsx_pivot_cache_rec_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_cache_rec_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_pivot_cache_rec_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_pivot_cache_rec_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_pivot_cache_rec_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    warn_unhandled();
}

bool xlsx_pivot_cache_rec_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_cache_rec_context::characters(const pstring& str, bool transient)
{
}

xlsx_pivot_table_context::xlsx_pivot_table_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_table_context::can_handle_element(xmlns_id_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_pivot_table_context::create_child_context(xmlns_id_t ns, xml_token_t name)
{
    return NULL;
}

void xlsx_pivot_table_context::end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_pivot_table_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    warn_unhandled();
}

bool xlsx_pivot_table_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_table_context::characters(const pstring& str, bool transient)
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
