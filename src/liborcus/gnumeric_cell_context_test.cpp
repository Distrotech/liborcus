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

#include "gnumeric_cell_context.hpp"
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

namespace {

class mock_sheet : public import_sheet
{
public:
    virtual void set_value(row_t row, col_t col, double val)
    {
        assert(row == 1);
        assert(col == 2);
        assert(val == 5.0);
    }

    virtual void set_bool(row_t row, col_t col, bool val)
    {
        assert(row == 31);
        assert(col == 32);
        assert(val == true);
    }

    virtual void set_string(row_t row, col_t col, size_t id)
    {
        assert(row == 10);
        assert(col == 321);
        assert(id == 2);
    }

    virtual void set_shared_formula(row_t row, col_t col, size_t id)
    {
        assert(row == 6);
        assert(col == 16);
        assert(id == 3);
    }

    virtual void set_shared_formula(row_t row, col_t col, formula_grammar_t grammar,
                                        size_t id, const char* s, size_t n)
    {
        assert(id == 2);
        assert(col == 15);
        assert(row == 5);
        assert(grammar == spreadsheet::gnumeric);
        assert(string(s, n) == "=basicFormulaString");
    }

    virtual void set_formula(row_t row, col_t col, formula_grammar_t grammar,
                                        const char* s, size_t n)
    {
        assert(row == 9);
        assert(col == 11);
        assert(grammar == gnumeric);
        assert(string(s, n) == "=formula");
    }

    virtual void set_array_formula(row_t row, col_t col, formula_grammar_t grammar,
            const char* s, size_t n, row_t rows, col_t cols)
    {
        assert(row == 19);
        assert(col == 111);
        assert(grammar == gnumeric);
        assert(string(s, n) == "=arrayFormula");
        assert(rows == 2);
        assert(cols == 3);
    }
};

class mock_shared_strings : public import_shared_strings
{
public:
    virtual size_t add(const char* s, size_t n)
    {
        assert(n == 14);
        assert(string(s, n) == "14 char string");
        return 2;
    }

};

class mock_factory : public import_factory
{
public:
    virtual iface::import_shared_strings* get_shared_strings()
    {
        return &m_shared_strings;
    }

private:
    mock_shared_strings m_shared_strings;
};

void test_cell_value()
{
    mock_sheet sheet;
    import_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "1", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "2", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ValueType, "40", false));
    context.start_element(ns, elem, attrs);
    context.characters("5", false);
    context.end_element(ns, elem);
}

void test_cell_bool()
{
    mock_sheet sheet;
    import_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "31", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "32", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ValueType, "20", false));
    context.start_element(ns, elem, attrs);
    context.characters("TRUE", false);
    context.end_element(ns, elem);
}

void test_cell_string()
{
    mock_sheet sheet;
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "10", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "321", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ValueType, "60", false));
    context.start_element(ns, elem, attrs);
    context.characters("14 char string", false);
    context.end_element(ns, elem);
}

void test_shared_formula_with_string()
{
    mock_sheet sheet;
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;

    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "5", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "15", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ExprID, "2", false));

    context.start_element(ns, elem, attrs);
    context.characters("=basicFormulaString", false);
    context.end_element(ns, elem);
}

void test_shared_formula_without_string()
{
    mock_sheet sheet;
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;

    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "6", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "16", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ExprID, "3", false));

    context.start_element(ns, elem, attrs);
    context.end_element(ns, elem);
}

void test_cell_formula()
{
    mock_sheet sheet;
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "9", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "11", false));
    context.start_element(ns, elem, attrs);
    context.characters("=formula", false);
    context.end_element(ns, elem);
}

void test_cell_array_formula()
{
    mock_sheet sheet;
    mock_factory factory;
    session_context cxt;

    orcus::gnumeric_cell_context context(cxt, orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "19", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "111", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Rows, "2", false));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Cols, "3", false));
    context.start_element(ns, elem, attrs);
    context.characters("=arrayFormula", false);
    context.end_element(ns, elem);
}

}

int main()
{
    test_cell_value();
    test_cell_bool();
    test_cell_string();
    test_shared_formula_with_string();
    test_shared_formula_without_string();
    test_cell_formula();
    test_cell_array_formula();

    return EXIT_SUCCESS;
}
