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

void test_odf_fill(orcus::spreadsheet::import_styles &styles)
{
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
}

void test_odf_border(orcus::spreadsheet::import_styles &styles)
{
    assert(styles.get_border_count() == 8);

    /* Test that border style applies to all the sides when not specified */
    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Name1", &styles);
    size_t xf = style->xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
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
    assert(cell_border->top.style == orcus::spreadsheet::border_style_t::solid);
    assert(cell_border->bottom.style == orcus::spreadsheet::border_style_t::thin);
    assert(cell_border->left.style == orcus::spreadsheet::border_style_t::none);
    assert(cell_border->right.style == orcus::spreadsheet::border_style_t::dash_dot_dot);
    assert(cell_border->top.border_color.red == 0xff);
    assert(cell_border->bottom.border_color.green == 0xee);
    assert(cell_border->left.border_color.blue == 0x11);
    assert(cell_border->right.border_width.value == 0.22);
    assert(cell_border->bottom.border_width.value == 1.74);

    /*Test that border applies to the diagonal*/
    style = find_cell_style_by_name("Name3", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    border = cell_format->border;
    assert(cell_format);

    cell_border = styles.get_border(border);
    assert(cell_border->diagonal_bl_tr.style == orcus::spreadsheet::border_style_t::medium);
    assert(cell_border->diagonal_tl_br.style == orcus::spreadsheet::border_style_t::medium_dash_dot);
    assert(cell_border->diagonal_bl_tr.border_color.red == 0xff);
    assert(cell_border->diagonal_tl_br.border_color.green == 0x00);
    assert(cell_border->diagonal_tl_br.border_width.value == 0.74);
}

void test_odf_cell_protection(orcus::spreadsheet::import_styles& styles)
{
    /* Test that Cell is only protected and not hidden , Print Content is true */
    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Name5", &styles);
    size_t  xf = style->xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
    size_t protection = cell_format->protection;
    assert(cell_format);

    const orcus::spreadsheet::protection_t* cell_protection = styles.get_protection(protection);
    assert(cell_protection->locked == true);
    assert(cell_protection->hidden == true);
    assert(cell_protection->print_content == true);
    assert(cell_protection->formula_hidden == false);

    /* Test that Cell is  protected and formula is hidden , Print Content is false */
    style = find_cell_style_by_name("Name6", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    protection = cell_format->protection;
    assert(cell_format);

    cell_protection = styles.get_protection(protection);
    assert(cell_protection->locked == true);
    assert(cell_protection->hidden == false);
    assert(cell_protection->print_content == false);
    assert(cell_protection->formula_hidden == true);

    /* Test that Cell is not protected by any way, Print Content is false */
    style = find_cell_style_by_name("Name7", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    protection = cell_format->protection;
    assert(cell_format);

    cell_protection = styles.get_protection(protection);
    assert(cell_protection->locked == false);
    assert(cell_protection->hidden == false);
    assert(cell_protection->print_content == true);
    assert(cell_protection->formula_hidden == false);
}

void test_odf_font(orcus::spreadsheet::import_styles& styles)
{
    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Name8", &styles);
    size_t xf = style->xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
    size_t font = cell_format->font;
    assert(cell_format);

    const orcus::spreadsheet::font_t* cell_font = styles.get_font(font);
    assert(cell_font->name == "Liberation Sans");
    assert(cell_font->size == 24);
    assert(cell_font->bold == true);
    assert(cell_font->italic == true);
    assert(cell_font->underline_style == orcus::spreadsheet::underline_t::solid);
    assert(cell_font->underline_width == orcus::spreadsheet::underline_width_t::thick);
    assert(cell_font->underline_mode == orcus::spreadsheet::underline_mode_t::continuos);
    assert(cell_font->underline_type == orcus::spreadsheet::underline_type_t::none);
    assert(cell_font->underline_color.red == (int)0x80);
    assert(cell_font->underline_color.green == (int)0x80);
    assert(cell_font->underline_color.blue == (int)0x80);

    style = find_cell_style_by_name("Name9", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    font = cell_format->font;
    assert(cell_format);

    cell_font = styles.get_font(font);
    assert(cell_font->name == "Tahoma");
    assert(cell_font->size == 00);
    assert(cell_font->bold == true);
    assert(cell_font->italic == false);
    assert(cell_font->underline_style == orcus::spreadsheet::underline_t::dash);
    assert(cell_font->underline_width == orcus::spreadsheet::underline_width_t::bold);
    assert(cell_font->underline_mode == orcus::spreadsheet::underline_mode_t::continuos);
    assert(cell_font->underline_type == orcus::spreadsheet::underline_type_t::none);
    assert(cell_font->underline_color.red == (int)0x18);
    assert(cell_font->underline_color.green == (int)0x56);
    assert(cell_font->underline_color.blue == (int)0xff);
}

void test_odf_number_formatting(orcus::spreadsheet::import_styles& styles)
{
    const orcus::spreadsheet::cell_style_t* style = find_cell_style_by_name("Name10", &styles);
    size_t xf = style->xf;
    const orcus::spreadsheet::cell_format_t* cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    size_t number_format = cell_format->number_format;
    const orcus::spreadsheet::number_format_t* cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "#.000000");

    style = find_cell_style_by_name("Name11", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "[$₹]#,##0.00;[RED]-[$₹]#,##0.00");

    style = find_cell_style_by_name("Name12", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "0.00%");

    style = find_cell_style_by_name("Name13", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "#.00E+00");

    style = find_cell_style_by_name("Name15", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "BOOLEAN");

    style = find_cell_style_by_name("Name16", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "#### ?/11");

    style = find_cell_style_by_name("Name17", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "MM/DD/YY");

    style = find_cell_style_by_name("Name18", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    std::cerr<<cell_number_format->format_string.str();
    assert(cell_number_format->format_string.str() == "HH:MM:SS AM/PM");

    style = find_cell_style_by_name("Name19", &styles);
    xf = style->xf;
    cell_format = styles.get_cell_style_format(xf);
    assert(cell_format);

    number_format = cell_format->number_format;
    cell_number_format = styles.get_number_format(number_format);
    assert(cell_number_format->format_string.str() == "[>=0]0.00;[RED]-0.00");

}
int main()
{
    orcus::string_pool string_pool;
    const char* path = SRCDIR"/test/ods/styles/cell-styles.xml";
    std::string content = orcus::load_file_content(path);
    orcus::spreadsheet::import_styles styles(string_pool);
    orcus::import_ods::read_styles(content.c_str(), content.size(), &styles);

    test_odf_fill(styles);
    test_odf_border(styles);
    test_odf_cell_protection(styles);
    test_odf_font(styles);

    orcus::string_pool string_pool2;
    path = SRCDIR"/test/ods/styles/number-format.xml";
    std::string content2 = orcus::load_file_content(path);
    orcus::spreadsheet::import_styles styles2(string_pool2);
    orcus::import_ods::read_styles(content2.c_str(), content2.size(), &styles2);

    test_odf_number_formatting(styles2);

    return 0;
}
