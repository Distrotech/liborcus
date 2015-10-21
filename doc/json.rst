.. highlight:: cpp

JSON
====

The JSON part of orcus consists of a low-level parser class that handles
parsing of JSON strings and a document class that stores parsed JSON
structures.

There are two approaches to process JSON strings using the orcus library.  One
approach is to utilize the :cpp:class:`~orcus::json_document_tree` class to
load and populate the JSON structure via the
:cpp:func:`~orcus::json_document_tree::load()` method and traverse its content
via the :cpp:func:`~orcus::json_document_tree::get_document_root()` method.
This method is handy if you want a quick way to parse and access the content
of the JSON structure with minimal effort.

Another approach is to use the low-level :cpp:class:`~orcus::json_parser`
class directly by providing your own handler class to receive callbacks from
the parser.  This method requires a bit more effort on your part to provide
and populate your own data structure, but if you already have a data structure
to store the content of JSON, then this approach is ideal.

Example
-------

Using the document tree
```````````````````````

::

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

        std::vector<orcus::pstring> keys = root.keys();

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

You'll see the following output when executing this code::

    name: John Doe
    occupation: Software Engineer
    score:
      - 89
      - 67
      - 90

Using the low-level parser
``````````````````````````

Public interface
----------------

Parser
``````

.. doxygenclass:: orcus::json_parser
   :members:

Document tree
`````````````

.. doxygenclass:: orcus::json_document_tree
   :members:

.. doxygenstruct:: orcus::json_config
   :members:

.. doxygenclass:: orcus::json::detail::node
   :members:

