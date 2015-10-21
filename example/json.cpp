
#include <orcus/json_document_tree.hpp>
#include <orcus/config.hpp>
#include <orcus/pstring.hpp>

#include <cstdlib>
#include <iostream>

using namespace std;

const char* json_string = "{"
"   \"name\": \"John Doe\","
"   \"occupation\": \"Software Engineer\","
"   \"score\": [89, 67, 90]"
"}";

int main()
{
    using node = orcus::json_document_tree::node;

    orcus::json_config config; // Use default configuration.

    orcus::json_document_tree doc;
    doc.load(json_string, config);

    // Root is an object containing three key-value pairs.
    node root = doc.get_document_root();

    vector<orcus::pstring> keys = root.keys();

    for (auto it = keys.begin(), ite = keys.end(); it != ite; ++it)
    {
        orcus::pstring key = *it;
        node value = root.child(key);
        switch (value.type())
        {
            case orcus::json_node_t::string:
                // string value
                cout << key << ": " << value.string_value() << endl;
            break;
            case orcus::json_node_t::array:
            {
                // array value
                cout << key << ":" << endl;

                for (size_t i = 0; i < value.child_count(); ++i)
                {
                    node array_element = value.child(i);
                    cout << "  - " << array_element.numeric_value() << endl;
                }
            }
            break;
            default:
                ;
        }
    }

    return EXIT_SUCCESS;
}
