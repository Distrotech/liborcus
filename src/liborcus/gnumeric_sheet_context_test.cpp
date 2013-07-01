/*************************************************************************
 *
 * Copyright (c) 2013 Kohei Yoshida
 * Copyright (c) 2013 Markus Mohrhard
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

#include "gnumeric_sheet_context.hpp"
#include "gnumeric_tokens.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_token_constants.hpp"
#include "mock_spreadsheet.hpp"
#include "session_context.hpp"
#include "orcus/types.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

using namespace orcus;
using namespace std;
using namespace orcus::spreadsheet;
using namespace orcus::spreadsheet::mock;

class mock_sheet_properties : public import_sheet_properties
{
public:
    virtual void set_column_width(col_t col, double size, length_unit_t unit)
    {
        assert(col == 2);
        assert(size == 37.3);
        assert(unit == length_unit_point);
    }

    virtual void set_column_hidden(col_t, bool)
    {
    }

    virtual void set_row_height(row_t row, double size, length_unit_t unit)
    {
        assert(row == 4);
        assert(size == 7.3);
        assert(unit == length_unit_point);
    }

    virtual void set_row_hidden(row_t, bool)
    {
    }
};

class mock_sheet : public import_sheet
{
public:
    virtual iface::import_sheet_properties* get_sheet_properties()
    {
        return &m_mock_properties;
    }

private:
    mock_sheet_properties m_mock_properties;
};

class mock_factory : public import_factory
{
public:
    virtual iface::import_sheet* append_sheet(const char*, size_t)
    {
        return &m_mock_sheet;
    }
private:
    mock_sheet m_mock_sheet;
};

void test_column_width()
{
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_sheet_context context(cxt, orcus::gnumeric_tokens, &factory);
    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t parent = XML_Sheet;
    orcus::xml_attrs_t parent_attr;
    context.start_element(ns, parent, parent_attr);
    {
        orcus::xml_token_t elem = XML_Name;
        orcus::xml_attrs_t attrs;
        context.start_element(ns, elem, attrs);
        context.characters("test", false);
        context.end_element(ns, elem);
    }
    {
        orcus::xml_token_t elem = XML_ColInfo;
        orcus::xml_attrs_t attrs;
        attrs.push_back(xml_token_attr_t(ns, XML_No, "2"));
        attrs.push_back(xml_token_attr_t(ns, XML_Unit, "37.3"));
        attrs.push_back(xml_token_attr_t(ns, XML_Unit, "37.3"));
        context.start_element(ns, elem, attrs);
        context.end_element(ns, elem);
    }
    context.end_element(ns, parent);
}

int main()
{
    test_column_width();

    return EXIT_SUCCESS;
}
