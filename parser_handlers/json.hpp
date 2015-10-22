/*************************************************************************
 *
 * Copyright (c) 2015 Kohei Yoshida
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_JSON_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_JSON_HPP

#include <orcus/css_types.hpp>

class json_parser_handler
{
public:
    /**
     * Called when the parsing begins.
     */
    void begin_parse() {}

    /**
     * Called when the parsing ends.
     */
    void end_parse() {}

    /**
     * Called when the opening brace of an array is encountered.
     */
    void begin_array() {}

    /**
     * Called when the closing brace of an array is encountered.
     */
    void end_array() {}

    /**
     * Called when the opening curly brace of an object is encountered.
     */
    void begin_object() {}

    /**
     * Called when a key value string of an object is encountered.
     *
     * @param p pointer to the first character of the key value string.
     * @param len length of the key value string.
     * @param transient true if the string value is stored in a temporary
     *                  buffer which is not guaranteed to hold the string
     *                  value after the end of this callback. When false, the
     *                  pointer points to somewhere in the JSON stream being
     *                  parsed.
     */
    void object_key(const char* p, size_t len, bool transient) {}

    /**
     * Called when the closing curly brace of an object is encountered.
     */
    void end_object() {}

    /**
     * Called when a boolean 'true' keyword is encountered.
     */
    void boolean_true() {}

    /**
     * Called when a boolean 'false' keyword is encountered.
     */
    void boolean_false() {}

    /**
     * Called when a 'null' keyword is encountered.
     */
    void null() {}

    /**
     * Called when a string value is encountered.
     *
     * @param p pointer to the first character of the string value.
     * @param len length of the string value.
     * @param transient true if the string value is stored in a temporary
     *                  buffer which is not guaranteed to hold the string
     *                  value after the end of this callback. When false, the
     *                  pointer points to somewhere in the JSON stream being
     *                  parsed.
     */
    void string(const char* p, size_t len, bool transient) {}

    /**
     * Called when a numeric value is encountered.
     *
     * @param val numeric value.
     */
    void number(double val) {}
};

#endif
