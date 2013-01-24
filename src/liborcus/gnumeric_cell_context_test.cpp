#include "gnumeric_cell_context.hpp"
#include "gnumeric_tokens.hpp"
#include "gnumeric_namespace_types.hpp"
#include "gnumeric_token_constants.hpp"
#include "mock_spreadsheet.hpp"
#include "orcus/types.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

using namespace orcus;
using namespace std;
using namespace orcus::spreadsheet;
using namespace orcus::spreadsheet::mock::iface;

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

    virtual void set_string(row_t row, col_t col, size_t id)
    {
        assert(row == 10);
        assert(col == 321);
        assert(id = 2);
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

    orcus::gnumeric_cell_context context(orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "1"));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "2"));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ValueType, "40"));
    context.start_element(ns, elem, attrs);
    context.characters("5");
    context.end_element(ns, elem);
}

void test_cell_string()
{
    mock_sheet sheet;
    mock_factory factory;

    orcus::gnumeric_cell_context context(orcus::gnumeric_tokens, &factory, &sheet);

    orcus::xmlns_id_t ns = NS_gnumeric_gnm;
    orcus::xml_token_t elem = XML_Cell;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Row, "10"));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_Col, "321"));
    attrs.push_back(xml_token_attr_t(NS_gnumeric_gnm, XML_ValueType, "60"));
    context.start_element(ns, elem, attrs);
    context.characters("14 char string");
    context.end_element(ns, elem);
}

}

int main()
{
    test_cell_value();
    test_cell_string();

    return EXIT_SUCCESS;
}
