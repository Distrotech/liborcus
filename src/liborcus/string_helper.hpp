#include <vector>

#include <orcus/pstring.hpp>

namespace orcus {

class string_helper
{
public:
    static std::vector<pstring> split_string(const pstring& string, const char separator);
};

}
