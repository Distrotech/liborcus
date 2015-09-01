#include "string_helper.hpp"

namespace orcus {

std::vector<pstring> string_helper::split_string(const pstring& str, const char sep)
{
    std::vector<pstring> ret;

    size_t len = 0;
    const char* start = str.get();
    for (size_t i = 0, n = str.size(); i < n; ++i)
    {
        if (str[i] == sep)
        {
            ret.push_back( pstring( start, len ) );

            // if not at the end move the start string
            if (i < n-1)
                start = start + len + 1;

            len = 0;
        }
        else
            ++len;
    }
    ret.push_back( pstring( start, len ) );

    return ret;
}

}
