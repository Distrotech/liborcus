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

bool xlsx_pivot_cache_def_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_pivot_cache_def_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_pivot_cache_def_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
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
            cout << "---" << endl;
            cout << "pivot cache definition" << endl;
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

void xlsx_pivot_cache_def_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

xlsx_pivot_cache_rec_context::xlsx_pivot_cache_rec_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_cache_rec_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_pivot_cache_rec_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_pivot_cache_rec_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_pivot_cache_rec_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    if (ns != NS_ooxml_xlsx)
        return;

    switch (name)
    {
        case XML_pivotCacheRecords:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
            long count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
            cout << "---" << endl;
            cout << "pivot cache record (count: " << count << ")" << endl;
        }
        break;
        case XML_r:
            xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotCacheRecords);
            cout << "* record" << endl;
        break;
        case XML_x:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_r);
            long v = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_v);
            cout << "  * x = " << v << endl;
        }
        break;
        case XML_n:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_r);
            double val = single_double_attr_getter::get(attrs, NS_ooxml_xlsx, XML_v);
            cout << "  * n = " << val << endl;
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_pivot_cache_rec_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_cache_rec_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

namespace {

class pivot_table_def_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        long v = 0;
        bool b = false;
        switch (attr.name)
        {
            case XML_name:
                cout << "name: " << attr.value << endl;
            break;
            case XML_cacheId:
                v = to_long(attr.value);
                cout << "cache ID: " << v << endl;
            break;
            case XML_applyNumberFormats:
                b = to_bool(attr.value);
                cout << "apply number formats: " << b << endl;
            break;
            case XML_applyBorderFormats:
                b = to_bool(attr.value);
                cout << "apply border formats: " << b << endl;
            break;
            case XML_applyFontFormats:
                b = to_bool(attr.value);
                cout << "apply font formats: " << b << endl;
            break;
            case XML_applyPatternFormats:
                b = to_bool(attr.value);
                cout << "apply pattern formats: " << b << endl;
            break;
            case XML_applyAlignmentFormats:
                b = to_bool(attr.value);
                cout << "apply alignment formats: " << b << endl;
            break;
            case XML_applyWidthHeightFormats:
                b = to_bool(attr.value);
                cout << "apply width/height formats: " << b << endl;
            break;
            case XML_dataCaption:
                cout << "data caption: " << attr.value << endl;
            break;
            case XML_updatedVersion:
                v = to_long(attr.value);
                cout << "updated version: " << v << endl;
            break;
            case XML_minRefreshableVersion:
                v = to_long(attr.value);
                cout << "minimum refreshable version: " << v << endl;
            break;
            case XML_showCalcMbrs:
                b = to_bool(attr.value);
                cout << "show calc members (?): " << b << endl;
            break;
            case XML_useAutoFormatting:
                b = to_bool(attr.value);
                cout << "use auto formatting: " << b << endl;
            break;
            case XML_itemPrintTitles:
                b = to_bool(attr.value);
                cout << "item print titles (?): " << b << endl;
            break;
            case XML_createdVersion:
                v = to_long(attr.value);
                cout << "created version: " << v << endl;
            break;
            case XML_indent:
                b = to_bool(attr.value);
                cout << "indent: " << b << endl;
            break;
            case XML_outline:
                b = to_bool(attr.value);
                cout << "outline: " << b << endl;
            break;
            case XML_outlineData:
                b = to_bool(attr.value);
                cout << "outline data: " << b << endl;
            break;
            case XML_multipleFieldFilters:
                b = to_bool(attr.value);
                cout << "multiple field filters: " << b << endl;
            break;
            default:
                ;
        }
    }
};

class location_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        long v = -1;
        switch (attr.name)
        {
            case XML_ref:
                cout << "ref: " << attr.value << endl;
            break;
            case XML_firstHeaderRow:
                v = to_long(attr.value);
                cout << "first header row: " << v << endl;
            break;
            case XML_firstDataRow:
                v = to_long(attr.value);
                cout << "first data row: " << v << endl;
            break;
            case XML_firstDataCol:
                v = to_long(attr.value);
                cout << "first data column: " << v << endl;
            break;
            default:
                ;
        }
    }
};

class pivot_field_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_axis:
                cout << "  * axis: " << attr.value << endl;
            break;
            case XML_showAll:
            {
                bool b = to_bool(attr.value);
                cout << "  * show all: " << b << endl;
            }
            break;
            default:
                ;
        }
    }
};

class data_field_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_name:
            {
                cout << "  * name = " << attr.value << endl;
            }
            break;
            case XML_fld:
            {
                long fld = to_long(attr.value);
                cout << "  * field = " << fld << endl;
            }
            break;
            case XML_baseField:
            {
                long fld = to_long(attr.value);
                cout << "  * base field = " << fld << endl;
            }
            break;
            case XML_baseItem:
            {
                long fld = to_long(attr.value);
                cout << "  * base item = " << fld << endl;
            }
            break;
            default:
                ;
        }
    }
};

class item_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_x:
            {
                long idx = to_long(attr.value);
                cout << "    * x = " << idx << endl;
            }
            break;
            case XML_t:
            {
                cout << "    * type = " << attr.value << endl;
            }
            break;
            default:
                ;
        }
    }
};

class i_attr_parser : public unary_function<xml_token_attr_t, void>
{
public:
    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        switch (attr.name)
        {
            case XML_t:
            {
                cout << "  * type = " << attr.value << endl;
            }
            break;
            default:
                ;
        }
    }
};

class pt_style_info_attr_parser : public unary_function<xml_token_attr_t, void>
{
    bool m_first;

    void sep()
    {
        if (m_first)
            m_first = false;
        else
            cout << ";";
    }

public:
    pt_style_info_attr_parser() : m_first(true) {}

    void operator() (const xml_token_attr_t& attr)
    {
        if (attr.ns != NS_ooxml_xlsx)
            return;

        bool b = false;
        switch (attr.name)
        {
            case XML_name:
                sep();
                cout << " name='" << attr.value << "'";
            break;
            case XML_showRowHeaders:
                b = to_bool(attr.value);
                sep();
                cout << " show row headers=" << b;
            break;
            case XML_showColHeaders:
                b = to_bool(attr.value);
                sep();
                cout << " show column headers=" << b;
            break;
            case XML_showRowStripes:
                b = to_bool(attr.value);
                sep();
                cout << " show row stripes=" << b;
            break;
            case XML_showColStripes:
                b = to_bool(attr.value);
                sep();
                cout << " show column stripes=" << b;
            break;
            case XML_showLastColumn:
                b = to_bool(attr.value);
                sep();
                cout << " show last column=" << b;
            break;
            default:
                ;
        }
    }
};


}

xlsx_pivot_table_context::xlsx_pivot_table_context(session_context& cxt, const tokens& tokens) :
    xml_context_base(cxt, tokens) {}

bool xlsx_pivot_table_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_pivot_table_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_pivot_table_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_pivot_table_context::start_element(xmlns_id_t ns, xml_token_t name, const::std::vector<xml_token_attr_t>& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    if (ns == NS_ooxml_xlsx)
    {
        switch (name)
        {
            case XML_pivotTableDefinition:
            {
                xml_element_expected(parent, XMLNS_UNKNOWN_ID, XML_UNKNOWN_TOKEN);
                cout << "---" << endl;
                pivot_table_def_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_location:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                location_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_pivotFields:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "field count: " << count << endl;
            }
            break;
            case XML_pivotField:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotFields);
                cout << "---" << endl;
                pivot_field_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_items:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotField);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "  * item count: " << count << endl;
            }
            break;
            case XML_item:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_items);
                item_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_rowFields:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "---" << endl;
                cout << "row field count: " << count << endl;
            }
            break;
            case XML_colFields:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "---" << endl;
                cout << "column field count: " << count << endl;
            }
            break;
            case XML_field:
            {
                xml_elem_stack_t expected;
                expected.reserve(3);
                expected.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_rowFields));
                expected.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_colFields));
                xml_element_expected(parent, expected);

                size_t idx = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_x);
                cout << "  * x = " << idx << endl;
            }
            break;
            case XML_dataFields:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "---" << endl;
                cout << "data field count: " << count << endl;
            }
            break;
            case XML_dataField:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_dataFields);
                data_field_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_rowItems:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "---" << endl;
                cout << "row item count: " << count << endl;
            }
            break;
            case XML_colItems:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                size_t count = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_count);
                cout << "---" << endl;
                cout << "column item count: " << count << endl;
            }
            break;
            case XML_i:
            {
                xml_elem_stack_t expected;
                expected.reserve(2);
                expected.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_rowItems));
                expected.push_back(xml_token_pair_t(NS_ooxml_xlsx, XML_colItems));
                xml_element_expected(parent, expected);

                i_attr_parser func;
                for_each(attrs.begin(), attrs.end(), func);
            }
            break;
            case XML_x:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_i);
                long idx = single_long_attr_getter::get(attrs, NS_ooxml_xlsx, XML_v);
                if (idx >= 0)
                    cout << "  * v = " << idx << endl;
                else
                    cout << "  * v not set" << endl;
            }
            break;
            case XML_pivotTableStyleInfo:
            {
                xml_element_expected(parent, NS_ooxml_xlsx, XML_pivotTableDefinition);
                pt_style_info_attr_parser func;
                cout << "  * style info:";
                for_each(attrs.begin(), attrs.end(), func);
                cout << endl;
            }
            break;
            default:
                warn_unhandled();
        }
    }
}

bool xlsx_pivot_table_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_pivot_table_context::characters(const pstring& /*str*/, bool /*transient*/)
{
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
