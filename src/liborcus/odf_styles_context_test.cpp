#include <orcus/orcus_import_ods.hpp>

#include <orcus/spreadsheet/styles.hpp>

#include <orcus/string_pool.hpp>
#include "orcus/stream.hpp"

#include <cassert>
#include <iostream>
#include <orcus/spreadsheet/types.hpp>
#include <orcus/measurement.hpp>

namespace {

const orcus::spreadsheet::cell_style_t* find_cell_style_by_name(const orcus::pstring& name, orcus::spreadsheet::import_styles* styles)
{
    size_t n = styles->get_cell_styles_count();
    for (size_t i = 0; i < n; ++i)
    {
        const orcus::spreadsheet::cell_style_t* cur_style = styles->get_cell_style(i);
        if (cur_style->name == name)
            return cur_style;
    }

    assert(false);
    return NULL;
}

}

int main()
{
    orcus::string_pool string_pool;
    const char* path = SRCDIR"/test/ods/styles/cell-styles.xml";
    std::string content = orcus::load_file_content(path);
    orcus::spreadsheet::import_styles styles(string_pool);
    orcus::import_ods::read_styles(content.c_str(), content.size(), &styles);

/*  Test for Cell Fill
    =====================================================
*/
    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Name1", &styles);
    assert(style->parent_name == "Text");
    size_t xf = style->xf;
    std::cerr << std::hex << (int)xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    size_t fill = cell_format->fill;
    std::cerr << std::hex << (int)fill;
    const orcus::spreadsheet::fill_t* cell_fill = styles.get_fill(fill);
    assert(cell_fill);
    std::cerr << std::hex << (int)cell_fill->bg_color.red;
    assert(cell_fill->bg_color.red == 0xfe);
    assert(cell_fill->bg_color.green == 0xff);
    assert(cell_fill->bg_color.blue == 0xcc);

/*  Test for Border Styles
    =====================================================
*/
    assert(styles.get_border_count() == 5);

    /* Test that border style applies to all the sides when not specified */
    style = find_cell_style_by_name("Name1", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    size_t border = cell_format->border;
    assert(cell_format);

    const orcus::spreadsheet::border_t* cell_border = styles.get_border(border);
    assert(cell_border->top.style == orcus::spreadsheet::border_style_t::thick);
    assert(cell_border->bottom.style == orcus::spreadsheet::border_style_t::thick);
    assert(cell_border->left.style == orcus::spreadsheet::border_style_t::thick);
    assert(cell_border->right.style == orcus::spreadsheet::border_style_t::thick);
    assert(cell_border->top.border_color.red == 0xff);
    assert(cell_border->bottom.border_color.green == 0xcc);
    assert(cell_border->left.border_color.blue == 0x12);
    assert(cell_border->right.border_width.value == 0.06);
    assert(cell_border->top.border_width.value == 0.06);

    /*Test that border applies to only specified sides*/
    style = find_cell_style_by_name("Name2", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    border = cell_format->border;
    assert(cell_format);

    cell_border = styles.get_border(border);
    assert(cell_border->top.style == orcus::spreadsheet::border_style_t::dashed);
    assert(cell_border->bottom.style == orcus::spreadsheet::border_style_t::thin);
    assert(cell_border->left.style == orcus::spreadsheet::border_style_t::none);
    assert(cell_border->right.style == orcus::spreadsheet::border_style_t::thin);
    assert(cell_border->top.border_color.red == 0xff);
    assert(cell_border->bottom.border_color.green == 0xee);
    assert(cell_border->left.border_color.blue == 0x11);
    assert(cell_border->right.border_width.value == 0.22);
    assert(cell_border->bottom.border_width.value == 1.74);

    //TODO : These border styles dont work :- solid,dash_dot,dash_dot_dot ...and some others

    return 0;
}
