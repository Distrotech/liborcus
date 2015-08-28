#include <orcus/orcus_import_ods.hpp>

#include <orcus/spreadsheet/styles.hpp>

#include <orcus/string_pool.hpp>
#include "orcus/stream.hpp"

#include <cassert>

int main()
{
    orcus::string_pool string_pool;
    const char* path = SRCDIR"/test/ods/styles/cell-styles.xml";
    std::string content = orcus::load_file_content(path);
    orcus::spreadsheet::import_styles styles(string_pool);
    orcus::import_ods::read_styles(content.c_str(), content.size(), &styles);

    assert(styles.get_fill(0));
    return 0;
}
