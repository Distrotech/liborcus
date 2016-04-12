/*************************************************************************
 *
 * Copyright (c) 2016 Kohei Yoshida
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

#ifndef INCLUDED_ORCUS_PARSER_HANDLERS_YAML_HPP
#define INCLUDED_ORCUS_PARSER_HANDLERS_YAML_HPP

#include <cstdlib>

class yaml_parser_handler
{
public:
    /**
     * Called when the parser starts parsing a content.
     */
    void begin_parse() {}

    /**
     * Called when the parser finishes parsing an entire content.
     */
    void end_parse() {}

    /**
     * Called when a new document is encountered.
     */
    void begin_document() {}

    /**
     * Called when the parser has finished parsing a document.
     */
    void end_document() {}

    /**
     * Called when a sequence begins.
     */
    void begin_sequence() {}

    /**
     * Called when a sequence ends.
     */
    void end_sequence() {}

    /**
     * Called when a map begins.
     */
    void begin_map() {}

    /**
     * Called when the parser starts parsing a map key.
     */
    void begin_map_key() {}

    /**
     * Called when the parser finishes parsing a map key.
     */
    void end_map_key() {}

    /**
     * Called when the parser finishes parsing an entire map.
     */
    void end_map() {}

    /**
     * Called when a string value is encountered.
     *
     * @param p pointer to the first character of the string value.
     * @param len length of the string value.
     */
    void string(const char* p, size_t n) {}

    /**
     * Called when a numeric value is encountered.
     *
     * @param val numeric value.
     */
    void number(double val) {}

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
};

#endif
