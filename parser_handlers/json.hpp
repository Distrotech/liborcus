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

/**
 * Skeleton handler for json_parser.  Feel free to copy this as a starting
 * point for your own JSON handler.
 */
class json_parser_handler
{
public:
    void begin_parse() {}

    void end_parse() {}

    void begin_array() {}

    void end_array() {}

    void begin_object() {}

    void object_key(const char* /*p*/, size_t /*len*/, bool /*transient*/) {}

    void end_object() {}

    void boolean_true() {}

    void boolean_false() {}

    void null() {}

    void string(const char* /*p*/, size_t /*len*/, bool /*transient*/) {}

    void number(double /*val*/) {}
};

#endif
