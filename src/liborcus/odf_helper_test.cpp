#include "odf_helper.hpp"

#include <orcus/spreadsheet/types.hpp>

#include <cassert>

using namespace orcus::spreadsheet;

namespace {

void test_color_conversion(const char* input, bool valid,
        color_elem_t red_expected, color_elem_t green_expected, color_elem_t blue_expected)
{
    color_elem_t red, green, blue;
    bool valid_result = orcus::odf_helper::convert_fo_color(orcus::pstring(input),
            red, green, blue);

    assert(valid == valid_result);
    if (valid)
    {
        assert(red_expected == red);
        assert(green_expected == green);
        assert(blue_expected == blue);
    }
}

}

int main()
{
    struct
    {
        const char* input;
        bool valid;
        orcus::spreadsheet::color_elem_t red;
        orcus::spreadsheet::color_elem_t green;
        orcus::spreadsheet::color_elem_t blue;
    } data[] = {
        { "not valid", false, 0, 0, 0},
        { "#000000", true, 0, 0, 0},
        { "#0000", false, 0, 0, 0},
        { "#abcdef", true, 0xab, 0xcd, 0xef},
        { "#ABCDEF", true, 0xab, 0xcd, 0xef},
        { "#123456", true, 0x12, 0x34, 0x56}
    };

    for (size_t i = 0; i < sizeof(data)/sizeof(data[0]); ++i)
    {
        test_color_conversion(data[i].input, data[i].valid, data[i].red, data[i].green, data[i].blue);
    }
    return 0;
}
