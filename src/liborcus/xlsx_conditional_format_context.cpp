/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "xlsx_conditional_format_context.hpp"
#include "xlsx_helper.hpp"
#include "xlsx_types.hpp"
#include "ooxml_token_constants.hpp"
#include "ooxml_namespace_types.hpp"

#include "orcus/global.hpp"
#include "orcus/exception.hpp"
#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/measurement.hpp"

#include <mdds/sorted_string_map.hpp>
#include <mdds/global.hpp>

namespace orcus {

namespace {

class color_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
public:
    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_rgb:
            {
                to_rgb(attr.value, m_alpha, m_red, m_green, m_blue);
            }
            break;
            default:
            break;
        }
    }

    void get_color(argb_color& color)
    {
        color.alpha = m_alpha;
        color.red = m_red;
        color.green = m_green;
        color.blue = m_blue;
    }

private:
    spreadsheet::color_elem_t m_alpha;
    spreadsheet::color_elem_t m_red;
    spreadsheet::color_elem_t m_green;
    spreadsheet::color_elem_t m_blue;
};

enum xlsx_cond_format_type
{
    none = 0,
    expression,
    cellIs,
    colorScale,
    dataBar,
    iconSet,
    top10,
    uniqueValues,
    duplicateValues,
    containsText,
    notContainsText,
    beginsWith,
    endsWith,
    containsBlanks,
    notContainsBlanks,
    containsErrors,
    notContainsErrors,
    timePeriod,
    aboveAverage
};

enum xlsx_cond_format_operator
{
    operator_default = 0,
    operator_beginsWith,
    operator_between,
    operator_containsText,
    operator_endsWith,
    operator_equal,
    operator_greaterThan,
    operator_greaterThanOrEqual,
    operator_lessThan,
    operator_lessThanOrEqual,
    operator_notBetween,
    operator_notContains,
    operator_notEqual
};

enum xlsx_cond_format_date
{
    date_default = 0,
    date_last7Days,
    date_lastMonth,
    date_lastWeek,
    date_nextMonth,
    date_thisMonth,
    date_thisWeek,
    date_today,
    date_tomorrow,
    date_yesterday
};

enum xlsx_cond_format_boolean
{
    boolean_default = 0,
    boolean_true,
    boolean_false
};

typedef mdds::sorted_string_map<xlsx_cond_format_type> cond_format_type_map;

typedef mdds::sorted_string_map<xlsx_cond_format_boolean> cond_format_boolean_map;

typedef mdds::sorted_string_map<xlsx_cond_format_operator> cond_format_operator_map;

typedef mdds::sorted_string_map<xlsx_cond_format_date> cond_format_date_map;

cond_format_type_map::entry cond_format_type_entries[] =
{
    { MDDS_ASCII("aboveAverage"), aboveAverage },
    { MDDS_ASCII("beginsWith"), beginsWith },
    { MDDS_ASCII("cellIs"), cellIs },
    { MDDS_ASCII("colorScale"), colorScale },
    { MDDS_ASCII("containsBlanks"), containsBlanks },
    { MDDS_ASCII("containsErrors"), containsErrors },
    { MDDS_ASCII("containsText"), containsText },
    { MDDS_ASCII("dataBar"), dataBar },
    { MDDS_ASCII("duplicateValues"), duplicateValues },
    { MDDS_ASCII("endsWith"), endsWith },
    { MDDS_ASCII("expression"), expression },
    { MDDS_ASCII("iconSet"), iconSet },
    { MDDS_ASCII("notContainsErrors"), notContainsErrors },
    { MDDS_ASCII("notContainsText"), notContainsText },
    { MDDS_ASCII("timePeriod"), timePeriod },
    { MDDS_ASCII("top10"), top10 },
    { MDDS_ASCII("uniqueValues"), uniqueValues }
};

cond_format_operator_map::entry cond_format_operator_entries[] =
{
    { MDDS_ASCII("beginsWith"), operator_beginsWith },
    { MDDS_ASCII("between"), operator_between },
    { MDDS_ASCII("containsText"), operator_containsText },
    { MDDS_ASCII("endsWith"), operator_endsWith },
    { MDDS_ASCII("equal"), operator_equal },
    { MDDS_ASCII("greaterThan"), operator_greaterThan },
    { MDDS_ASCII("greaterThanOrEqual"), operator_greaterThanOrEqual },
    { MDDS_ASCII("lessThan"), operator_lessThan },
    { MDDS_ASCII("lessThanOrEqual"), operator_lessThanOrEqual },
    { MDDS_ASCII("notBetween"), operator_notBetween },
    { MDDS_ASCII("notContains"), operator_notContains },
    { MDDS_ASCII("notEqual"), operator_notEqual }
};

cond_format_date_map::entry cond_format_date_entries[] =
{
    { MDDS_ASCII("last7Days"), date_last7Days },
    { MDDS_ASCII("lastMonth"), date_lastMonth },
    { MDDS_ASCII("lastWeek"), date_lastWeek },
    { MDDS_ASCII("nextMonth"), date_nextMonth },
    { MDDS_ASCII("thisMonth"), date_thisMonth },
    { MDDS_ASCII("thisWeek"), date_thisWeek },
    { MDDS_ASCII("today"), date_today },
    { MDDS_ASCII("tomorrow"), date_tomorrow },
    { MDDS_ASCII("yesterday"), date_yesterday }
};

cond_format_boolean_map::entry cond_format_boolean_entries[] =
{
    { MDDS_ASCII("0"), boolean_false },
    { MDDS_ASCII("1"), boolean_true },
    { MDDS_ASCII("false"), boolean_true },
    { MDDS_ASCII("true"), boolean_false }
};

bool parse_boolean_flag(const xml_token_attr_t& attr, bool default_value)
{
    static const cond_format_boolean_map boolean_map(cond_format_boolean_entries, sizeof(cond_format_boolean_entries)/sizeof(cond_format_boolean_entries[0]), boolean_default);    
    xlsx_cond_format_boolean val = boolean_map.find(attr.value.get(), attr.value.size());
    switch (val)
    {
        case boolean_default:
            return default_value;
        break;
        case boolean_true:
            return true;
        break;
        case boolean_false:
            return false;
        break;
    }

    return default_value;
}

struct cfRule_attr_parser : public std::unary_function<xml_token_attr_t, void>
{

    cfRule_attr_parser(spreadsheet::iface::import_conditional_format& cond_format):
        m_cond_format(cond_format),
        m_type(none),
        m_operator(operator_default),
        m_date(date_default),
        m_above_average(true),
        m_equal_average(false),
        m_percent(false),
        m_bottom(false)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch(attr.name)
        {
            case XML_type:
            {
                cond_format_type_map type_map(cond_format_type_entries, sizeof(cond_format_type_entries)/sizeof(cond_format_type_entries[0]), none);
                m_type = type_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_dxfId:
            {
                // TODO: actually we need to translate between dxf id and xf id
                size_t dxf_id = to_long(attr.value);
                m_cond_format.set_xf_id(dxf_id);
            }
            break;
            case XML_aboveAverage:
            {
                m_above_average = parse_boolean_flag(attr, true);
            }
            break;
            case XML_percent:
                m_percent = parse_boolean_flag(attr, false);
            break;
            case XML_bottom:
                m_bottom = parse_boolean_flag(attr, false);
            break;
            case XML_operator:
            {
                cond_format_operator_map operator_map(cond_format_operator_entries, sizeof(cond_format_operator_entries)/sizeof(cond_format_operator_entries[0]), operator_default);
                m_operator = operator_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_text:
                // do we need to worry about the transient flag here?
                m_text = attr.value;
            break;
            case XML_timePeriod:
            {
                cond_format_date_map date_map(cond_format_date_entries, sizeof(cond_format_date_entries)/sizeof(cond_format_date_entries[0]), date_default);
                m_date = date_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_rank:
                // do we need to worry about the transient flag here?
                m_rank = attr.value;
            break;
            case XML_stdDev:
                // do we need to worry about the transient flag here?
                m_std_dev = attr.value;
            break;
            case XML_equalAverage:
                m_equal_average = parse_boolean_flag(attr, false);
            break;
            default:
                break;
        }
    }

    void set_type()
    {
        switch (m_type)
        {
            case expression:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::expression);
            break;
            case cellIs:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::expression);
                switch (m_operator)
                {
                    case operator_beginsWith:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::begins_with);
                    break;
                    case operator_between:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::between);
                    break;
                    case operator_containsText:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::contains);
                    break;
                    case operator_endsWith:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::ends_with);
                    break;
                    case operator_equal:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::equal);
                    break;
                    case operator_greaterThan:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::greater);
                    break;
                    case operator_greaterThanOrEqual:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::greater_equal);
                    break;
                    case operator_lessThan:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::less);
                    break;
                    case operator_lessThanOrEqual:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::less_equal);
                    break;
                    case operator_notBetween:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::not_between);
                    break;
                    case operator_notContains:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::not_contains);
                    break;
                    case operator_notEqual:
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::not_equal);
                    break;
                    default:
                    break;
                }
            break;
            case colorScale:
                m_cond_format.set_type(spreadsheet::conditional_format_t::colorscale);
            break;
            case dataBar:
                m_cond_format.set_type(spreadsheet::conditional_format_t::databar);
            break;
            case iconSet:
                m_cond_format.set_type(spreadsheet::conditional_format_t::iconset);
            break;
            case top10:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                if (m_bottom)
                {
                    m_cond_format.set_operator(spreadsheet::condition_operator_t::bottom_n);
                }
                else
                {
                    m_cond_format.set_operator(spreadsheet::condition_operator_t::top_n);
                }
                m_cond_format.set_formula(m_rank.get(), m_rank.size());
            break;
            case uniqueValues:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::unique);
            break;
            case duplicateValues:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::duplicate);
            break;
            case containsText:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::contains);
                m_cond_format.set_formula(m_text.get(), m_text.size());
            break;
            case notContainsText:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::not_contains);
                m_cond_format.set_formula(m_text.get(), m_text.size());
            break;
            case beginsWith:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::begins_with);
                m_cond_format.set_formula(m_text.get(), m_text.size());
            break;
            case endsWith:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::ends_with);
                m_cond_format.set_formula(m_text.get(), m_text.size());
            break;
            case containsBlanks:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::contains_blanks);
            break;
            case containsErrors:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::contains_error);
            break;
            case notContainsErrors:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                m_cond_format.set_operator(spreadsheet::condition_operator_t::contains_no_error);
            break;
            case timePeriod:
                m_cond_format.set_type(spreadsheet::conditional_format_t::date);
                switch (m_date)
                {
                    case date_last7Days:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::last_7_days);
                    break;
                    case date_lastMonth:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::last_month);
                    break;
                    case date_lastWeek:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::last_week);
                    break;
                    case date_nextMonth:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::next_month);
                    break;
                    case date_thisMonth:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::this_month);
                    break;
                    case date_thisWeek:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::this_week);
                    break;
                    case date_today:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::today);
                    break;
                    case date_tomorrow:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::tomorrow);
                    break;
                    case date_yesterday:
                        m_cond_format.set_date(orcus::spreadsheet::condition_date_t::yesterday);
                    break;
                    default:
                    break;
                }
            break;
            case aboveAverage:
                m_cond_format.set_type(spreadsheet::conditional_format_t::condition);
                if (!m_std_dev.empty())
                {
                    // TODO: we need a way to mark that as std dev in the interfaces
                    m_cond_format.set_formula(m_std_dev.get(), m_std_dev.size());
                }
                if (m_above_average)
                {
                    if (m_equal_average)
                    {
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::above_equal_average);
                    }
                    else
                    {
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::above_average);
                    }
                }
                else
                {
                    if (m_equal_average)
                    {
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::below_equal_average);
                    }
                    else
                    {
                        m_cond_format.set_operator(spreadsheet::condition_operator_t::below_average);
                    }
                }
            break;
            default:
            break;
        }
    }

private:
    spreadsheet::iface::import_conditional_format& m_cond_format;
    xlsx_cond_format_type m_type;
    xlsx_cond_format_operator m_operator;
    xlsx_cond_format_date m_date;
    bool m_above_average;
    bool m_equal_average;
    bool m_percent;
    bool m_bottom;
    pstring m_text;
    pstring m_std_dev;
    pstring m_rank;
};

struct conditional_formatting_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    conditional_formatting_attr_parser(spreadsheet::iface::import_conditional_format& cond_format):
        m_cond_format(cond_format)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_sqref:
                m_cond_format.set_range(attr.value.get(), attr.value.size());
            break;
            default:
            break;
        }
    }

private:
    spreadsheet::iface::import_conditional_format& m_cond_format;
};

enum xlsx_cond_format_cfvo_type
{
    cfvo_default = 0,
    cfvo_num,
    cfvo_percent,
    cfvo_max,
    cfvo_min,
    cfvo_formula,
    cfvo_percentile
};

typedef mdds::sorted_string_map<xlsx_cond_format_cfvo_type> cond_format_cfvo_type_map;

cond_format_cfvo_type_map::entry cond_format_cfvo_entries[] =
{
    { MDDS_ASCII("num"), cfvo_num },
    { MDDS_ASCII("percent"), cfvo_percent },
    { MDDS_ASCII("max"), cfvo_max },
    { MDDS_ASCII("min"), cfvo_min },
    { MDDS_ASCII("formula"), cfvo_formula },
    { MDDS_ASCII("percentile"), cfvo_percentile },
};

}

struct cfvo_values
{
    cfvo_values():
        m_include_equal(true),
        m_type(cfvo_default)
    {
    }

    bool m_include_equal;
    xlsx_cond_format_cfvo_type m_type;
    pstring m_value;
};

namespace {

struct cfvo_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    cfvo_attr_parser(string_pool& pool):
        m_string_pool(pool)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_gte:
                m_values.m_include_equal = parse_boolean_flag(attr, true);
            break;
            case XML_type:
            {
                cond_format_cfvo_type_map cfvo_type_map(cond_format_cfvo_entries, sizeof(cond_format_cfvo_entries)/sizeof(cond_format_cfvo_entries[0]), cfvo_default);
                m_values.m_type = cfvo_type_map.find(attr.value.get(), attr.value.size());
            }
            break;
            case XML_val:
                if (attr.transient)
                {
                    m_values.m_value = m_string_pool.intern(attr.value).first;
                }
                else
                {
                    m_values.m_value = attr.value;
                }
            break;
            default:
            break;
        }
    }

    cfvo_values get_values()
    {
        return m_values;
    }

private:
    cfvo_values m_values;
    string_pool& m_string_pool;
};

struct data_bar_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    data_bar_attr_parser():
        m_show_value(true),
        m_min_length(10),
        m_max_length(90)
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_showValue:
                m_show_value = parse_boolean_flag(attr, true);
            break;
            case XML_maxLength:
                m_max_length = to_long(attr.value);
            break;
            case XML_minLength:
                m_min_length = to_long(attr.value);
            break;
            default:
            break;
        }
    }

    void import_data(spreadsheet::iface::import_conditional_format& cond_format)
    {
        cond_format.set_show_value(m_show_value);
        cond_format.set_min_databar_length(m_min_length);
        cond_format.set_max_databar_length(m_max_length);
    }

private:
    bool m_show_value;
    size_t m_min_length;
    size_t m_max_length;
};

struct icon_set_attr_parser : public std::unary_function<xml_token_attr_t, void>
{
    icon_set_attr_parser():
        m_reverse(false),
        m_percent(true),
        m_show_value(true),
        icon_name("3Arrows")
    {
    }

    void operator()(const xml_token_attr_t& attr)
    {
        switch (attr.name)
        {
            case XML_iconSet:
                icon_name = attr.value;
            break;
            case XML_percent:
                m_percent = parse_boolean_flag(attr, true);
            break;
            case XML_reverse:
                m_reverse = parse_boolean_flag(attr, false);
            break;
            case XML_showValue:
                m_show_value = parse_boolean_flag(attr, true);
            break;
            default:
            break;
        }
    }

    void import_data(spreadsheet::iface::import_conditional_format& cond_format)
    {
        cond_format.set_show_value(m_show_value);
        cond_format.set_iconset_reverse(m_reverse);
        cond_format.set_icon_name(icon_name.get(), icon_name.size());
    }

private:
    bool m_reverse;
    bool m_percent;
    bool m_show_value;
    pstring icon_name;
};

}

xlsx_conditional_format_context::xlsx_conditional_format_context(
        session_context& session_cxt, const tokens& tokens,
        spreadsheet::iface::import_conditional_format& import_cond_format):
    xml_context_base(session_cxt, tokens),
    m_cond_format(import_cond_format)
{
}

xlsx_conditional_format_context::~xlsx_conditional_format_context()
{
}

bool xlsx_conditional_format_context::can_handle_element(xmlns_id_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_conditional_format_context::create_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/)
{
    return NULL;
}

void xlsx_conditional_format_context::end_child_context(xmlns_id_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_conditional_format_context::start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    switch (name)
    {
        case XML_conditionalFormatting:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_worksheet);
            std::for_each(attrs.begin(), attrs.end(), conditional_formatting_attr_parser(m_cond_format));
        }
        break;
        case XML_cfRule:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_conditionalFormatting);
            cfRule_attr_parser parser = std::for_each(attrs.begin(), attrs.end(), cfRule_attr_parser(m_cond_format));
            parser.set_type();
        }
        break;
        case XML_cfvo:
        {
            cfvo_attr_parser parser = std::for_each(attrs.begin(), attrs.end(), cfvo_attr_parser(m_pool));
            m_cfvo_values.push_back(parser.get_values());
        }
        break;
        case XML_dataBar:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cfRule);
            data_bar_attr_parser parser = std::for_each(attrs.begin(), attrs.end(), data_bar_attr_parser());
            parser.import_data(m_cond_format);
        }
        break;
        case XML_iconSet:
        {
            xml_element_expected(parent, NS_ooxml_xlsx, XML_cfRule);
            icon_set_attr_parser parser = std::for_each(attrs.begin(), attrs.end(), icon_set_attr_parser());
            parser.import_data(m_cond_format);
        }
        break;
        case XML_color:
        {
            color_attr_parser func = for_each(attrs.begin(), attrs.end(), color_attr_parser());
            argb_color color;
            func.get_color(color);
            m_colors.push_back(color);
        }
        break;
        case XML_formula:
        break;
        case XML_colorScale:
        break;
        default:
            warn_unhandled();
    }
}

namespace {

void import_cfvo(const cfvo_values& values, spreadsheet::iface::import_conditional_format& cond_format)
{
    if (!values.m_value.empty())
        cond_format.set_formula(values.m_value.get(),values.m_value.size());
    switch (values.m_type)
    {
        case cfvo_num:
            cond_format.set_condition_type(spreadsheet::condition_type_t::value);
        break;
        case cfvo_percent:
            cond_format.set_condition_type(spreadsheet::condition_type_t::percent);
        break;
        case cfvo_max:
            cond_format.set_condition_type(spreadsheet::condition_type_t::max);
        break;
        case cfvo_min:
            cond_format.set_condition_type(spreadsheet::condition_type_t::min);
        break;
        case cfvo_formula:
            cond_format.set_condition_type(spreadsheet::condition_type_t::formula);
        break;
        case cfvo_percentile:
            cond_format.set_condition_type(spreadsheet::condition_type_t::percentile);
        break;
        default:
        break;
    }
}

}

bool xlsx_conditional_format_context::end_element(xmlns_id_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_conditionalFormatting:
        {
            m_cond_format.commit_format();
        }
        break;
        case XML_cfRule:
        {
            m_cond_format.commit_entry();
            m_cfvo_values.clear();
            m_colors.clear();
        }
        break;
        case XML_cfvo:
        break;
        case XML_color:
        break;
        case XML_formula:
        {
            m_cond_format.set_formula(m_cur_str.get(), m_cur_str.size());
            m_cond_format.commit_condition();
        }
        break;
        case XML_dataBar:
        {
            if (m_colors.size() != 1)
                throw general_error("invalid dataBar record");
            if (m_cfvo_values.size() != 2)
                throw general_error("invalid dataBar record");
            argb_color& color = m_colors[0];
            m_cond_format.set_databar_color_positive(color.alpha, color.red,
                    color.green, color.blue);
            m_cond_format.set_databar_color_negative(color.alpha, color.red,
                    color.green, color.blue);
            for (std::vector<cfvo_values>::const_iterator itr = m_cfvo_values.begin(); itr != m_cfvo_values.end(); ++itr) {
                import_cfvo(*itr, m_cond_format);
                m_cond_format.commit_condition();
            }
        }
        break;
        case XML_iconSet:
            if (m_cfvo_values.size() < 2)
                throw general_error("invalid iconSet record");
            for (std::vector<cfvo_values>::const_iterator itr = m_cfvo_values.begin(); itr != m_cfvo_values.end(); ++itr) {
                import_cfvo(*itr, m_cond_format);
                m_cond_format.commit_condition();
            }
        break;
        case XML_colorScale:
        {
            if (m_cfvo_values.size() < 2)
                throw general_error("invalid colorScale record");
            if (m_cfvo_values.size() != m_colors.size())
                throw general_error("invalid colorScale record");
            std::vector<argb_color>::const_iterator itrColor = m_colors.begin();
            for (std::vector<cfvo_values>::const_iterator itr = m_cfvo_values.begin(); itr != m_cfvo_values.end(); ++itr, ++itrColor) {
                import_cfvo(*itr, m_cond_format);
                m_cond_format.set_color(itrColor->alpha, itrColor->red,
                        itrColor->green, itrColor->blue);
                m_cond_format.commit_condition();
            }
        }
        break;
        default:
            ;
    }

    m_cur_str.clear();
    return pop_stack(ns, name);
}

void xlsx_conditional_format_context::characters(const pstring& str, bool transient)
{
    m_cur_str = str;
    if (transient)
        m_cur_str = m_pool.intern(m_cur_str).first;
}


}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
