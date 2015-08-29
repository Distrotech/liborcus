#include <orcus/orcus_import_ods.hpp>

#include <orcus/spreadsheet/styles.hpp>

#include <orcus/string_pool.hpp>
#include "orcus/stream.hpp"

#include <cassert>
#include <iostream>

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


    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Note", &styles);
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
    return 0;
}
