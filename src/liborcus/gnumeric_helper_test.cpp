#include "gnumeric_helper.hpp"
#include "gnumeric_helper.cpp"

#include <iostream>

namespace {

/*
void test_split_string()
{
    orcus::pstring str("str1:str2:str3");
    std::vector<orcus::pstring> res = string_helper::split_string(str, ':');
    assert(res.size() == 3);
    assert(res[0] == "str1");
    assert(res[1] == "str2");
    assert(res[2] == "str3");
}
*/

void test_parse_color_string()
{
    orcus::pstring str("8080");
    size_t res = orcus::parse_color_string(str);
    std::cout << res << std::endl;
    assert(res == 128);

    res = orcus::parse_color_string(orcus::pstring("FFFF"));
    assert(res == 255);

    res = orcus::parse_color_string(orcus::pstring("0"));
    assert(res == 0);
}

}

int main()
{
    test_parse_color_string();
}
