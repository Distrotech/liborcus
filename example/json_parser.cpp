
#include <orcus/json_parser.hpp>
#include <orcus/pstring.hpp>
#include <cstring>
#include <iostream>

using namespace std;

class json_parser_handler
{
public:
    void begin_parse()
    {
        cout << "begin parse" << endl;
    }

    void end_parse()
    {
        cout << "end parse" << endl;
    }

    void begin_array()
    {
        cout << "begin array" << endl;
    }

    void end_array()
    {
        cout << "end array" << endl;
    }

    void begin_object()
    {
        cout << "begin object" << endl;
    }

    void object_key(const char* p, size_t len, bool transient)
    {
        cout << "object key: " << orcus::pstring(p, len) << endl;
    }

    void end_object()
    {
        cout << "end object" << endl;
    }

    void boolean_true()
    {
        cout << "true" << endl;
    }

    void boolean_false()
    {
        cout << "false" << endl;
    }

    void null()
    {
        cout << "null" << endl;
    }

    void string(const char* p, size_t len, bool transient)
    {
        cout << "string: " << orcus::pstring(p, len) << endl;
    }

    void number(double val)
    {
        cout << "number: " << val << endl;
    }
};

int main()
{
    const char* test_code = "{\"key1\": [1,2,3,4,5], \"key2\": 12.3}";
    size_t n = strlen(test_code);

    cout << "JSON string: " << test_code << endl;

    // Instantiate the parser with an own handler.
    json_parser_handler hdl;
    orcus::json_parser<json_parser_handler> parser(test_code, n, hdl);

    // Parse the string.
    parser.parse();

    return EXIT_SUCCESS;
}
