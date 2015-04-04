
#include "css.hpp"
#include <orcus/css_parser.hpp>

#include <cstring>

int main()
{
    const char* test_code = "p { background-color: white; }";
    size_t n = strlen(test_code);

    orcus::css_parser_handler hdl;
    orcus::css_parser<orcus::css_parser_handler> parser(test_code, n, hdl);
    parser.parse();
    return 0;
}
