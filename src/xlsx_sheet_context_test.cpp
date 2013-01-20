#include "mock/spreadsheet-mock.hpp"
#include "liborcus/ooxml_namespace_types.hpp"
#include "liborcus/ooxml_tokens.hpp"
#include "liborcus/ooxml_schemas.hpp"
#include "liborcus/xlsx_sheet_context.hpp"
#include "liborcus/ooxml_token_constants.hpp"
#include "orcus/types.hpp"

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
        assert(row == 0);
        assert(col == 0);
        assert(val == 5.0);
    }

};

void test_cell_value()
{
    mock_sheet sheet;

    orcus::xlsx_sheet_context context(orcus::ooxml_tokens, &sheet);

    orcus::xmlns_id_t ns = NS_ooxml_xlsx;
    orcus::xml_token_t elem = XML_c;
    orcus::xml_attrs_t attrs;
    attrs.push_back(xml_token_attr_t(NS_ooxml_xlsx, XML_r, "A1"));
    context.start_element(ns, elem, attrs);

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
    return 0;
}
