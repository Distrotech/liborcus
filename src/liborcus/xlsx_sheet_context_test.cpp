#include "mock_spreadsheet.hpp"
#include "ooxml_namespace_types.hpp"
#include "ooxml_tokens.hpp"
#include "ooxml_schemas.hpp"
#include "xlsx_sheet_context.hpp"
#include "ooxml_token_constants.hpp"
#include "session_context.hpp"
#include "orcus/types.hpp"

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
        assert(row == -1);
        assert(col == 0);
        assert(val == 5.0);
    }

    virtual void set_bool(row_t row, col_t col, bool val)
    {
        assert(row == -1);
        assert(col == 0);
        assert(val == true);
    }

    virtual void set_array_formula(row_t row, col_t col, formula_grammar_t grammar,
            const char* s, size_t n, const char* s_range, size_t n_range)
    {
        assert(row == -1);
        assert(col == 0);
        assert(grammar == xlsx_2007);
        assert(string(s, n) == "A1:A2");
        assert(string(s_range, n_range) == "B3:B4");
    }

};

void test_cell_value()
{
    mock_sheet sheet;
    session_context cxt;

    orcus::xlsx_sheet_context context(cxt, orcus::ooxml_tokens, &sheet);

    orcus::xmlns_id_t ns = NS_ooxml_xlsx;
    orcus::xml_token_t elem = XML_c;
    orcus::xml_attrs_t attrs;
    context.start_element(ns, elem, attrs);

    {
        xml_attrs_t val_attrs;
        context.start_element(ns, XML_v, val_attrs);
        context.characters("5");
        context.end_element(ns, XML_v);
    }

    context.end_element(ns, elem);
}

void test_cell_bool()
{
    mock_sheet sheet;
    session_context cxt;

    orcus::xlsx_sheet_context context(cxt, orcus::ooxml_tokens, &sheet);

    orcus::xmlns_id_t ns = NS_ooxml_xlsx;
    orcus::xml_token_t elem = XML_c;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_ooxml_xlsx, XML_t, "b"));
    context.start_element(ns, elem, attrs);

    {
        xml_attrs_t val_attrs;
        context.start_element(ns, XML_v, val_attrs);
        context.characters("1");
        context.end_element(ns, XML_v);
    }

    context.end_element(ns, elem);
}

void test_array_formula()
{
    mock_sheet sheet;
    session_context cxt;

    orcus::xlsx_sheet_context context(cxt, orcus::ooxml_tokens, &sheet);

    orcus::xmlns_id_t ns = NS_ooxml_xlsx;
    orcus::xml_token_t elem = XML_c;
    orcus::xml_attrs_t attrs;
    context.start_element(ns, elem, attrs);

    {
        xml_attrs_t formula_attrs;
        formula_attrs.push_back(xml_token_attr_t(NS_ooxml_xlsx, XML_t, "array"));
        formula_attrs.push_back(xml_token_attr_t(NS_ooxml_xlsx, XML_ref, "B3:B4"));
        context.start_element(ns, XML_f, formula_attrs);
        context.characters("A1:A2");
        context.end_element(ns, XML_f);
    }
    {
        xml_attrs_t val_attrs;
        context.start_element(ns, XML_v, val_attrs);
        context.characters("5");
        context.end_element(ns, XML_v);
    }

    context.end_element(ns, elem);
}

}

int main()
{
    test_cell_value();
    test_cell_bool();
    test_array_formula();
    return 0;
}
