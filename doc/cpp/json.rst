.. highlight:: cpp

JSON
====

The JSON part of orcus consists of a low-level parser class that handles
parsing of JSON strings and a document class that stores parsed JSON
structures.

There are two approaches to process JSON strings using the orcus library.  One
approach is to utilize the :cpp:class:`~orcus::json_document_tree` class to
load and populate the JSON structure tree via its
:cpp:func:`~orcus::json_document_tree::load()` method and traverse the tree
through its :cpp:func:`~orcus::json_document_tree::get_document_root()` method.
This approach is ideal if you want a quick way to parse and access the content
of a JSON document with minimal effort.

The other approach is to use the low-level :cpp:class:`~orcus::json_parser`
class directly by providing your own handler class to receive callbacks from
the parser.  This method requires a bit more effort on your part to provide
and populate your own data structure, but if you already have a data structure
to store the content of JSON, then this approach is ideal.  The
:cpp:class:`~orcus::json_document_tree` class internally uses
:cpp:class:`~orcus::json_parser` to parse JSON contents.

Example
-------

Populating a document tree
``````````````````````````
The following code snippet shows an example of how to populate an instance of
:cpp:class:`~orcus::json_document_tree` from a JSON string, and navigate its
content tree afterward.

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

        // Load JSON string into a document tree.
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

You'll see the following output when executing this code:

.. code-block:: text

    name: John Doe
    occupation: Software Engineer
    score:
      - 89
      - 67
      - 90

Using the low-level parser
``````````````````````````
The following code snippet shows how to use the low-level :cpp:class:`~orcus::json_parser`
class by providing an own handler class and passing it as a template argument::

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

Executing this code will generate the following output:

.. code-block:: text

    JSON string: {"key1": [1,2,3,4,5], "key2": 12.3}
    begin parse
    begin object
    object key: key1
    begin array
    number: 1
    number: 2
    number: 3
    number: 4
    number: 5
    end array
    object key: key2
    number: 12.3
    end object
    end parse


Public interface
----------------

Parser
``````

.. doxygenclass:: orcus::json_parser
   :members:

Parser handler
``````````````

.. doxygenclass:: json_parser_handler
   :members:

Document tree
`````````````

.. doxygenclass:: orcus::json_document_tree
   :members:

.. doxygenstruct:: orcus::json_config
   :members:

.. doxygenclass:: orcus::json::detail::node
   :members:

.. doxygenenum:: orcus::json::detail::node_t
   :project: orcus
