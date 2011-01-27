/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "orcus/ooxml/xlsx_context.hpp"
#include "orcus/global.hpp"
#include "orcus/ooxml/global.hpp"
#include "orcus/ooxml/ooxml_token_constants.hpp"
#include "orcus/ooxml/ooxml_types.hpp"
#include "orcus/ooxml/schemas.hpp"
#include "orcus/model/sheet.hpp"
#include "orcus/model/shared_strings.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

namespace orcus {

namespace {

class workbook_attr_parser : public root_element_attr_parser
{
public:
    workbook_attr_parser() :
        root_element_attr_parser(SCH_xlsx_main, XMLNS_xlsx) {}
    virtual ~workbook_attr_parser() {}
    virtual void handle_other_attrs(const xml_attr_t &attr) {}
};

class workbook_sheet_attr_parser : public unary_function<xml_attr_t, void>
{
public:
    void operator() (const xml_attr_t& attr)
    {
        if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_name)
            m_sheet.name = attr.value.intern();
        else if (attr.ns == XMLNS_UNKNOWN_TOKEN && attr.name == XML_sheetId)
        {
            const pstring& val = attr.value;
            if (!val.empty())
                m_sheet.id = strtoul(val.str().c_str(), NULL, 10);
        }
        else if (attr.ns == XMLNS_r && attr.name == XML_id)
        {
            m_rid = attr.value.intern();
        }
    }

    const xlsx_rel_sheet_info get_sheet() const { return m_sheet; }
    const pstring& get_rid() const { return m_rid; }

private:
    pstring m_rid;
    xlsx_rel_sheet_info m_sheet;
};

}

xlsx_workbook_context::xlsx_workbook_context(const tokens& tokens) :
    xml_context_base(tokens) {}

xlsx_workbook_context::~xlsx_workbook_context() {}

bool xlsx_workbook_context::can_handle_element(xmlns_token_t /*ns*/, xml_token_t /*name*/) const
{
    return true;
}

xml_context_base* xlsx_workbook_context::create_child_context(xmlns_token_t /*ns*/, xml_token_t /*name*/) const
{
    return NULL;
}

void xlsx_workbook_context::end_child_context(xmlns_token_t /*ns*/, xml_token_t /*name*/, xml_context_base* /*child*/)
{
}

void xlsx_workbook_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_workbook:
        {
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);

            xmlns_token_t default_ns =
                for_each(attrs.begin(), attrs.end(), workbook_attr_parser()).get_default_ns();

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_sheets:
            xml_element_expected(parent, XMLNS_xlsx, XML_workbook);
        break;
        case XML_sheet:
        {
            xml_element_expected(parent, XMLNS_xlsx, XML_sheets);
            workbook_sheet_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            m_sheets.push_back(new xlsx_rel_sheet_info(func.get_sheet()));
            const xlsx_rel_sheet_info& info = m_sheets.back();
            m_sheet_info.insert(
                opc_rel_extras_t::value_type(func.get_rid(), &info));
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_workbook_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_workbook_context::characters(const pstring& str) {}

void xlsx_workbook_context::pop_sheet_info(opc_rel_extras_t& sheets)
{
    m_sheet_info.swap(sheets);
}

namespace {

class worksheet_attr_parser : public root_element_attr_parser
{
public:
    worksheet_attr_parser() :
        root_element_attr_parser(SCH_xlsx_main, XMLNS_xlsx) {}
    virtual ~worksheet_attr_parser() {}
    virtual void handle_other_attrs(const xml_attr_t &attr) {}
};

class row_attr_parser : public unary_function<void, xml_attr_t>
{
public:
    row_attr_parser() : m_row(0) {}
    void operator() (const xml_attr_t& attr)
    {
        if (attr.name == XML_r)
        {
            // row index
            m_row = static_cast<model::row_t>(
                strtoul(attr.value.str().c_str(), NULL, 10));
            if (!m_row)
                throw xml_structure_error("row number can never be zero!");

            m_row -= 1; // from 1-based to 0-based.
        }
    }

    model::row_t get_row() const { return m_row; }

private:
    model::row_t m_row;
};

class cell_attr_parser : public unary_function<xml_attr_t, void>
{
    struct address
    {
        model::row_t row;
        model::col_t col;
        address(model::row_t _row, model::col_t _col) : row(_row), col(_col) {}
    };
public:
    cell_attr_parser() : m_type(xlsx_sheet_xml_context::cell_type_value), m_address(0,0) {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.name == XML_r)
        {
            // cell address in A1 notation.
            m_address = to_cell_address(attr.value);
        }
        else if (attr.name == XML_t)
        {
            // cell type
            m_type = to_cell_type(attr.value);
        }
    }

    xlsx_sheet_xml_context::cell_type get_cell_type() const { return m_type; }

    model::row_t get_row() const { return m_address.row; }
    model::col_t get_col() const { return m_address.col; }

private:
    xlsx_sheet_xml_context::cell_type to_cell_type(const pstring& s) const
    {
        // TODO: check if there are other cell types.
        xlsx_sheet_xml_context::cell_type t = xlsx_sheet_xml_context::cell_type_value;
        if (!s.empty() && s[0] == 's')
            t = xlsx_sheet_xml_context::cell_type_string;
        return t;
    }

    address to_cell_address(const pstring& s) const
    {
        model::row_t row = 0;
        model::col_t col = 0;
        const char* p = s.get();
        size_t n = s.size();
        for (size_t i = 0; i < n; ++i, ++p)
        {
            char c = *p;
            if ('A' <= c && c <= 'Z')
            {
                col *= 26;
                col += static_cast<model::col_t>(c - 'A' + 1);
            }
            else if ('0' <= c && c <= '9')
            {
                row *= 10;
                row += static_cast<model::row_t>(c - '0');
            }
            else
            {
                ostringstream os;
                os << "invalid cell address: " << s;
                throw xml_structure_error(os.str());
            }
        }

        if (!row || !col)
        {
            ostringstream os;
            os << "invalid cell address: " << s;
            throw xml_structure_error(os.str());
        }

        return address(row-1, col-1); // switch from 1-based to 0-based.
    }

private:
    xlsx_sheet_xml_context::cell_type m_type;
    address   m_address;
};

}

xlsx_sheet_xml_context::xlsx_sheet_xml_context(const tokens& tokens, model::sheet_base* sheet) :
    xml_context_base(tokens),
    mp_sheet(sheet),
    m_current_row(0)
{
}

xlsx_sheet_xml_context::~xlsx_sheet_xml_context()
{
}

bool xlsx_sheet_xml_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_sheet_xml_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void xlsx_sheet_xml_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_sheet_xml_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);

    switch (name)
    {
        case XML_worksheet:
        {
            print_attrs(get_tokens(), attrs);

            xmlns_token_t default_ns = 
                for_each(attrs.begin(), attrs.end(), worksheet_attr_parser()).get_default_ns();

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_cols:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_col:
            xml_element_expected(parent, XMLNS_xlsx, XML_cols);
        break;
        case XML_dimension:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_pageMargins:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_sheetViews:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_sheetView:
            xml_element_expected(parent, XMLNS_xlsx, XML_sheetViews);
        break;
        case XML_selection:
            xml_element_expected(parent, XMLNS_xlsx, XML_sheetView);
        break;
        case XML_sheetData:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_sheetFormatPr:
            xml_element_expected(parent, XMLNS_xlsx, XML_worksheet);
        break;
        case XML_row:
        {
            xml_element_expected(parent, XMLNS_xlsx, XML_sheetData);
            row_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            m_current_row = func.get_row();
        }
        break;
        case XML_c:
        {
            xml_element_expected(parent, XMLNS_xlsx, XML_row);
            cell_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);

            if (m_current_row != func.get_row())
                throw xml_structure_error("row numbers differ!");

            m_current_col = func.get_col();
            m_current_cell_type = func.get_cell_type();
        }
        break;
        case XML_v:
            xml_element_expected(parent, XMLNS_xlsx, XML_c);
        break;
        default:
            warn_unhandled();
    }

}

bool xlsx_sheet_xml_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_v:
        {
            switch (m_current_cell_type)
            {
                case cell_type_string:
                {
                    // string cell
                    size_t str_id = strtoul(m_current_str.str().c_str(), NULL, 10);
                    mp_sheet->set_string(m_current_row, m_current_col, str_id);
                }
                break;
                case cell_type_value:
                {
                    // value cell
                    double val = strtod(m_current_str.str().c_str(), NULL);
                    mp_sheet->set_value(m_current_row, m_current_col, val);
                }
                break;
            }
        }
        break;
    }

    return pop_stack(ns, name);
}

void xlsx_sheet_xml_context::characters(const pstring& str)
{
    m_current_str = str;
}

// ============================================================================

namespace {

class shared_strings_root_attr_parser : public root_element_attr_parser
{
public:
    shared_strings_root_attr_parser() :
        root_element_attr_parser(SCH_xlsx_main, XMLNS_xlsx),
        m_count(0), m_unique_count(0) {}
    virtual ~shared_strings_root_attr_parser() {}

    virtual void handle_other_attrs(const xml_attr_t &attr)
    {
        switch (attr.name)
        {
            case XML_count:
                m_count = strtoul(attr.value.str().c_str(), NULL, 10);
            break;
            case XML_uniqueCount:
                m_unique_count = strtoul(attr.value.str().c_str(), NULL, 10);
            break;
        }
    }

    shared_strings_root_attr_parser& operator= (const shared_strings_root_attr_parser& r)
    {
        root_element_attr_parser::operator= (r);
        m_count = r.m_count;
        m_unique_count = r.m_unique_count;
        return *this;
    }

    size_t get_count() const { return m_count; }
    size_t get_unique_count() const { return m_unique_count; }
private:
    size_t m_count;
    size_t m_unique_count;
};

/**
 * Use this when we need to just get the value of a "val" attribute.
 */
class val_attr_getter : public unary_function<xml_attr_t, void>
{
    pstring m_value;
public:
    val_attr_getter() {}

    void operator() (const xml_attr_t& attr)
    {
        if (attr.name == XML_val)
            m_value = attr.value;
    }

    const pstring& get_value() const { return m_value; }
};

}

xlsx_shared_strings_context::xlsx_shared_strings_context(const tokens& tokens, model::shared_strings_base* strings) :
    xml_context_base(tokens), mp_strings(strings), m_in_segments(false) {}

xlsx_shared_strings_context::~xlsx_shared_strings_context() {}

bool xlsx_shared_strings_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_shared_strings_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void xlsx_shared_strings_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_shared_strings_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_sst:
        {
            // root element for the shared string part.
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);

            shared_strings_root_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            xmlns_token_t default_ns = func.get_default_ns();

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);

            cout << "count: " << func.get_count() << "  unique count: " << func.get_unique_count() << endl;
        }
        break;
        case XML_si:
            // single shared string entry.
            m_in_segments = false;
            xml_element_expected(parent, XMLNS_xlsx, XML_sst);
        break;
        case XML_r:
            // rich text run
            m_in_segments = true;
            xml_element_expected(parent, XMLNS_xlsx, XML_si);
        break;
        case XML_rPr:
            // rich text run property
            xml_element_expected(parent, XMLNS_xlsx, XML_r);
        break;
        case XML_b:
            // bold
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
        break;
        case XML_i:
            // italic
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
        break;
        case XML_sz:
        {
            // font size
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
            const pstring& s = for_each(attrs.begin(), attrs.end(), val_attr_getter()).get_value();
            double point = strtod(s.str().c_str(), NULL);
            mp_strings->set_segment_font_size(point);
        }
        break;
        case XML_color:
            // data bar color
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
        break;
        case XML_rFont:
        {
            // font
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
            const pstring& font = for_each(attrs.begin(), attrs.end(), val_attr_getter()).get_value();
            mp_strings->set_segment_font(font.get(), font.size());
        }
        break;
        case XML_family:
            // font family
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
        break;
        case XML_scheme:
            // font scheme
            xml_element_expected(parent, XMLNS_xlsx, XML_rPr);
        break;
        case XML_t:
        {
            // actual text stored as its content.
            xml_elem_stack_t allowed;
            allowed.push_back(xml_token_pair_t(XMLNS_xlsx, XML_si));
            allowed.push_back(xml_token_pair_t(XMLNS_xlsx, XML_r));
            xml_element_expected(parent, allowed);
        }
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_shared_strings_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    switch (name)
    {
        case XML_t:
        break;
        case XML_b:
            mp_strings->set_segment_bold(true);
        break;
        case XML_i:
            mp_strings->set_segment_italic(true);
        break;
        case XML_r:
            mp_strings->append_segment(m_current_str.get(), m_current_str.size());
        break;
        case XML_si:
        {
            if (m_in_segments)
                // commit all formatted segments.
                mp_strings->commit_segments();
            else
            {
                // unformatted text should only have one text segment.
                mp_strings->append(m_current_str.get(), m_current_str.size());
            }
        }
        break;
    }
    return pop_stack(ns, name);
}

void xlsx_shared_strings_context::characters(const pstring& str)
{
    xml_token_pair_t& cur_token = get_current_element();
    if (cur_token.first == XMLNS_xlsx && cur_token.second == XML_t)
        m_current_str = str;
}

// ============================================================================

namespace {

class styles_root_attr_parser : public root_element_attr_parser
{
public:
    styles_root_attr_parser() :
        root_element_attr_parser(SCH_xlsx_main, XMLNS_xlsx) {}
};

}

xlsx_styles_context::xlsx_styles_context(const tokens& tokens) :
    xml_context_base(tokens) {}

xlsx_styles_context::~xlsx_styles_context() {}

bool xlsx_styles_context::can_handle_element(xmlns_token_t ns, xml_token_t name) const
{
    return true;
}

xml_context_base* xlsx_styles_context::create_child_context(xmlns_token_t ns, xml_token_t name) const
{
    return NULL;
}

void xlsx_styles_context::end_child_context(xmlns_token_t ns, xml_token_t name, xml_context_base* child)
{
}

void xlsx_styles_context::start_element(xmlns_token_t ns, xml_token_t name, const xml_attrs_t& attrs)
{
    xml_token_pair_t parent = push_stack(ns, name);
    switch (name)
    {
        case XML_styleSheet:
        {
            // root element
            xml_element_expected(parent, XMLNS_UNKNOWN_TOKEN, XML_UNKNOWN_TOKEN);
            print_attrs(get_tokens(), attrs);

            styles_root_attr_parser func;
            func = for_each(attrs.begin(), attrs.end(), func);
            xmlns_token_t default_ns = func.get_default_ns();

            // the namespace for worksheet element comes from its own 'xmlns' attribute.
            get_current_element().first = default_ns;
            set_default_ns(default_ns);
        }
        break;
        case XML_fonts:
        break;
        default:
            warn_unhandled();
    }
}

bool xlsx_styles_context::end_element(xmlns_token_t ns, xml_token_t name)
{
    return pop_stack(ns, name);
}

void xlsx_styles_context::characters(const pstring& str)
{
}

}
